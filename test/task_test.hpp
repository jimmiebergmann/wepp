#include "gtest/gtest.h"
#include <vector>
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
        EXPECT_TRUE(task2() == 123);

        Task<TestStruct> task3(123, "test");
        EXPECT_TRUE(task3().integer == 123);
        EXPECT_TRUE(task3().string == "test");

        Task<> task4;
        Task<> task5(true);
        EXPECT_TRUE(task5() == true);
        Task<> task6(false);
        EXPECT_TRUE(task6() == false);
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
        EXPECT_TRUE(taskCtrl3().integer == 123);
        EXPECT_TRUE(taskCtrl3().string == "test");

        TaskController<> taskCtrl4;
        TaskController<> taskCtrl5(true);
        EXPECT_TRUE(taskCtrl5() == true);
        TaskController<> taskCtrl6(false);
        EXPECT_TRUE(taskCtrl6() == false);
    }
    {
        TaskController<int> taskCtrl(0);
        EXPECT_EQ(taskCtrl.status(), Task<int>::Status::Pending);
        taskCtrl.finish();
        EXPECT_EQ(taskCtrl.status(), Task<int>::Status::Successful);
    }
    {
        TaskController<int> taskCtrl(0);
        EXPECT_EQ(taskCtrl.status(), Task<int>::Status::Pending);
        taskCtrl.fail();
        EXPECT_EQ(taskCtrl.status(), Task<int>::Status::Failed);
    }
    {
        TaskController<int> taskCtrl(1337);
        EXPECT_EQ(taskCtrl.status(), Task<int>::Status::Pending);
        EXPECT_EQ(taskCtrl(), 1337);

        Task<int> & task = taskCtrl;
        EXPECT_EQ(task.status(), Task<int>::Status::Pending);
        EXPECT_EQ(task(), 1337);

        taskCtrl() = 2999;
        taskCtrl.finish();

        EXPECT_EQ(taskCtrl.status(), Task<int>::Status::Successful);
        EXPECT_EQ(taskCtrl(), 2999);
        EXPECT_EQ(task.status(), Task<int>::Status::Successful);
        EXPECT_EQ(task(), 2999);
    }
}

TEST(Task, Wait)
{
    {
        auto func = [](const Task<>::Status status, const int value) -> Task<int>
        {
            TaskController<int> task(value);

            switch (status)
            {
            case Task<>::Status::Successful: task.finish(); break;
            case Task<>::Status::Failed: task.fail(); break;
            default: break;
            }

            return task;
        };

        {
            auto task = func(Task<>::Status::Successful, 123).wait();
            EXPECT_EQ(task.status(), Task<int>::Status::Successful);
            EXPECT_EQ(task(), 123);
        }
        {
            auto task = func(Task<>::Status::Failed, 404).wait();
            EXPECT_EQ(task.status(), Task<int>::Status::Failed);
            EXPECT_EQ(task(), 404);
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

            Task<int> run(const Task<>::Status status, const int value, const std::chrono::duration<double> & sleep)
            {
                TaskController<int> task(value);

                m_thread = std::thread([task, status, sleep]() mutable
                {
                    std::this_thread::sleep_for(sleep);

                    switch (status)
                    {
                    case Task<>::Status::Successful: task.finish(); break;
                    case Task<>::Status::Failed: task.fail(); break;
                    default: break;
                    }
                });

                return task;
            }

        private:

            std::thread m_thread;

        };

       /* {
            Waiter waiter;
            auto task = waiter.run(Task<>::Status::Successful, 123, std::chrono::seconds(3)).wait();
            EXPECT_EQ(task.status(), Task<int>::Status::Successful);
            EXPECT_EQ(task(), 123);
        }
        {
            Waiter waiter;

            auto start = std::chrono::high_resolution_clock::now();
            auto task = waiter.run(Task<>::Status::Failed, 404, std::chrono::seconds(3)).wait();
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;

            EXPECT_TRUE(elapsed.count() >= 3.0f);
            EXPECT_TRUE(elapsed.count() < 10.0f);
            EXPECT_EQ(task.status(), Task<int>::Status::Failed);
            EXPECT_EQ(task(), 404);
            
        }*/
    }
}
