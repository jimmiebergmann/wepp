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
        Task<int> task;


    }
}
