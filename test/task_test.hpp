#include "gtest/gtest.h"
#include "wepp/socket/socket.hpp"
#include "wepp/task.hpp"
#include <thread>

using namespace Wepp;


TEST(Task, Task)
{
    {
        struct TestStruct
        {
            TestStruct(int integer, const std::string & string) :
                integer(integer), string(string)
            { }

            int integer;
            std::string string;
        };

        Task<int> task1;
        Task<int> task2(123);
        EXPECT_EQ(task2(), 123);

        Task<TestStruct> task3(123, "test");
        EXPECT_EQ(task3().integer, 123);
        EXPECT_STREQ(task3().string.c_str(), "test");

        Task<> task4;
        Task<> task5(true);
        EXPECT_TRUE(task5());
        Task<> task6(false);
        EXPECT_FALSE(task6());
    }
}

TEST(Task, TaskController)
{
    {
        struct TestStruct
        {
            TestStruct(int integer, const std::string & string) :
                integer(integer), string(string)
            { }

            int integer;
            std::string string;
        };

        TaskController<int> taskCtrl1;
        TaskController<int> taskCtrl2(123);
        EXPECT_TRUE(taskCtrl2() == 123);

        TaskController<TestStruct> taskCtrl3(123, "test");
        EXPECT_EQ(taskCtrl3().integer, 123);
        EXPECT_STREQ(taskCtrl3().string.c_str(), "test");

        TaskController<> taskCtrl4;
        TaskController<> taskCtrl5(true);
        EXPECT_TRUE(taskCtrl5());
        TaskController<> taskCtrl6(false);
        EXPECT_FALSE(taskCtrl6());
    }
    {
        TaskController<int> taskCtrl(0);
        EXPECT_EQ(taskCtrl.status(), TaskStatus::Pending);
        taskCtrl.finish();
        EXPECT_EQ(taskCtrl.status(), TaskStatus::Successful);
    }
    {
        TaskController<int> taskCtrl(0);
        EXPECT_EQ(taskCtrl.status(), TaskStatus::Pending);
        taskCtrl.fail();
        EXPECT_EQ(taskCtrl.status(), TaskStatus::Failed);
    }
    {
        TaskController<int> taskCtrl(1337);
        EXPECT_EQ(taskCtrl.status(), TaskStatus::Pending);
        EXPECT_EQ(taskCtrl(), 1337);

        Task<int> & task = taskCtrl;
        EXPECT_EQ(task.status(), TaskStatus::Pending);
        EXPECT_EQ(task(), 1337);

        taskCtrl() = 2999;
        taskCtrl.finish();

        EXPECT_EQ(taskCtrl.status(), TaskStatus::Successful);
        EXPECT_EQ(taskCtrl(), 2999);
        EXPECT_EQ(task.status(), TaskStatus::Successful);
        EXPECT_EQ(task(), 2999);
    }
}

TEST(Task, Wait)
{
    {
        auto func = [](const TaskStatus status, const int value) -> Task<int>
        {
            TaskController<int> task(value);

            switch (status)
            {
            case TaskStatus::Successful: task.finish(); break;
            case TaskStatus::Failed: task.fail(); break;
            default: break;
            }

            return task;
        };

        {
            auto task = func(TaskStatus::Successful, 123).wait();
            EXPECT_EQ(task.status(), TaskStatus::Successful);
            EXPECT_TRUE(task.successful());
            EXPECT_FALSE(task.failed());
            EXPECT_FALSE(task.pending());
            EXPECT_EQ(task(), 123);
        }
        {
            auto task = func(TaskStatus::Failed, 404).wait();
            EXPECT_EQ(task.status(), TaskStatus::Failed);
            EXPECT_FALSE(task.successful());
            EXPECT_TRUE(task.failed());
            EXPECT_FALSE(task.pending());
            EXPECT_EQ(task(), 404);
        }
        {
            auto task = func(TaskStatus::Pending, 678);
            EXPECT_EQ(task.status(), TaskStatus::Pending);
            EXPECT_FALSE(task.successful());
            EXPECT_FALSE(task.failed());
            EXPECT_TRUE(task.pending());
            EXPECT_EQ(task(), 678);
        }
    }
    {

        class Waiter
        {

        public:

            ~Waiter()
            {
                if (m_thread.joinable())
                {
                    m_thread.join();
                }
            }

            Task<int> run(const TaskStatus status, const int value, const std::chrono::duration<double> & sleep)
            {
                TaskController<int> task(value);

                m_thread = std::thread([task, status, sleep]() mutable
                {
                    std::this_thread::sleep_for(sleep);

                    switch (status)
                    {
                    case TaskStatus::Successful: task.finish(); break;
                    default: task.fail(); break;
                    }
                });

                return task;
            }

        private:

            std::thread m_thread;

        };

        {
            Waiter waiter;

            auto task = waiter.run(TaskStatus::Successful, 123, std::chrono::seconds(3)).wait();
            EXPECT_EQ(task.status(), TaskStatus::Successful);
            EXPECT_EQ(task(), 123);
        }
        {
            Waiter waiter;

            auto task = waiter.run(TaskStatus::Failed, 456, std::chrono::seconds(3));
            EXPECT_EQ(task.status(), TaskStatus::Pending);
            EXPECT_FALSE(task.successful());
            EXPECT_FALSE(task.failed());
            EXPECT_TRUE(task.pending());

            task.wait();

            EXPECT_EQ(task.status(), TaskStatus::Failed);
            EXPECT_FALSE(task.successful());
            EXPECT_TRUE(task.failed());
            EXPECT_FALSE(task.pending());
            EXPECT_EQ(task(), 456);
        }
        {
            Waiter waiter;

            auto start = std::chrono::high_resolution_clock::now();
            auto task = waiter.run(TaskStatus::Failed, 404, std::chrono::seconds(2)).wait();
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;

            EXPECT_GE(elapsed.count(), 1.9);
            EXPECT_LT(elapsed.count(), 4.0f);
            EXPECT_EQ(task.status(), TaskStatus::Failed);
            EXPECT_EQ(task(), 404);

        }
        {
            Waiter waiter;

            auto start = std::chrono::high_resolution_clock::now();
            auto task = waiter.run(TaskStatus::Failed, 404, std::chrono::seconds(1)).wait(std::chrono::duration<double>(2));
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;

            EXPECT_GE(elapsed.count(), 0.9f);
            EXPECT_LT(elapsed.count(), 1.1f);
            EXPECT_FALSE(task.timeout());
            EXPECT_EQ(task.status(), TaskStatus::Failed);
            EXPECT_EQ(task(), 404);
        }
        {
            Waiter waiter;

            auto start = std::chrono::high_resolution_clock::now();
            auto task = waiter.run(TaskStatus::Failed, 404, std::chrono::seconds(2)).wait(std::chrono::duration<double>(1));
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;

            EXPECT_GE(elapsed.count(), 0.9f);
            EXPECT_LT(elapsed.count(), 1.1f);
            EXPECT_TRUE(task.timeout());
            std::cout << "Info: " << elapsed.count() << "  4s sleep, 2s timeout.";
            EXPECT_EQ(task.status(), TaskStatus::Pending);
            EXPECT_EQ(task(), 404);
        } 

        Socket::Socket::setLastError(0);
    }
}
