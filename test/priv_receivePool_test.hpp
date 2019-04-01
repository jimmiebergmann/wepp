#include "test.hpp"
#include "wepp/priv/receivePool.hpp"
#include <mutex>

using namespace Wepp;

TEST(Priv_ReciverPool, Start_Stop)
{
    {
        Priv::ReceivePool pool;
        GTEST_PRINT("Destroying not yet started recv pool 1.");
    }
    GTEST_PRINT("Destroyed not yet started recv pool 1.");
    {
        Priv::ReceivePool pool;
        GTEST_PRINT("Starting recv pool 2.");
        EXPECT_NO_THROW(pool.start(10, 20, [](Priv::HttpReceiver &, std::shared_ptr<Socket::TcpSocket>) {}));
        GTEST_PRINT("Destroying started recv pool 2.");
    }
    GTEST_PRINT("Destroyed started recv pool 2.");
    {
        Priv::ReceivePool pool;
        GTEST_PRINT("Starting recv pool 3.");
        EXPECT_TRUE(pool.start(10, 20, [](Priv::HttpReceiver &, std::shared_ptr<Socket::TcpSocket>) {}).wait().successful());
        GTEST_PRINT("Destroying started recv pool 3.");
    }
    GTEST_PRINT("Destroyed started recv pool 3.");
    {
        Priv::ReceivePool pool;
        GTEST_PRINT("Starting recv pool 4.");
        EXPECT_TRUE(pool.start(10, 20, [](Priv::HttpReceiver &, std::shared_ptr<Socket::TcpSocket>) {}).wait().successful());
        GTEST_PRINT("Stopping recv pool 4.");
        EXPECT_TRUE(pool.stop().wait().successful());
        GTEST_PRINT("Destroying stopped recv pool 4.");
    }
    GTEST_PRINT("Destroyed started recv pool 4.");
    {
        Priv::ReceivePool pool;
        GTEST_PRINT("Starting recv pool 5.");
        EXPECT_TRUE(pool.start(10, 20, [](Priv::HttpReceiver &, std::shared_ptr<Socket::TcpSocket>) {}).wait().successful());
        GTEST_PRINT("Starting recv pool 5 again.");
        EXPECT_TRUE(pool.start(10, 20, [](Priv::HttpReceiver &, std::shared_ptr<Socket::TcpSocket>) {}).wait().failed());
    }
    GTEST_PRINT("Destroyed started recv pool 5.");
    {
        Priv::ReceivePool pool;
        GTEST_PRINT("Stopping recv pool 6.");
        EXPECT_TRUE(pool.stop().wait().successful());
        GTEST_PRINT("Stopping recv pool 6 again.");
        EXPECT_TRUE(pool.stop().wait().successful());
    }
    GTEST_PRINT("Destroyed not started recv pool 6.");
    {
        Priv::ReceivePool pool;
        GTEST_PRINT("Stopping recv pool 7.");
        EXPECT_TRUE(pool.stop().wait().successful());
        GTEST_PRINT("Starting recv pool 7.");
        EXPECT_TRUE(pool.start(10, 20, [](Priv::HttpReceiver &, std::shared_ptr<Socket::TcpSocket>) {}).wait().successful());
        GTEST_PRINT("Starting recv pool 7 again.");
        EXPECT_TRUE(pool.start(10, 20, [](Priv::HttpReceiver &, std::shared_ptr<Socket::TcpSocket>) {}).wait().failed());
        GTEST_PRINT("Stopping recv pool 7 again.");
        EXPECT_TRUE(pool.stop().wait().successful());
    }
    GTEST_PRINT("Destroyed stopped recv pool 7.");
}


TEST(Priv_ReciverPool, Enqueue)
{
    for(size_t i = 1; i <= 100; i++)
    {
        Priv::ReceivePool pool;
        Semaphore semaphore;
        std::mutex mutex;
        size_t counter = 0;
        const size_t maxLoops = 10;

        EXPECT_TRUE(pool.start(i, i, [&mutex, &semaphore, &counter, maxLoops](Priv::HttpReceiver &, std::shared_ptr<Socket::TcpSocket>)
        {
            std::lock_guard<std::mutex> lock(mutex);

            counter++;
            if (counter == maxLoops)
            {
                semaphore.notifyOne();
            }
                
        }).wait().successful());


        for (size_t j = 0; j < 100; j++)
        {
            counter = 0;
            for (size_t k = 0; k < maxLoops; k++)
            {
                pool.enqueue(std::make_shared<Socket::TcpSocket>());
            }
            semaphore.wait();
            EXPECT_EQ(counter, maxLoops);
        }
       
    }

}