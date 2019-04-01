#include "test.hpp"
#include "wepp/socket/tcpListener.hpp"
#include <thread>

using namespace Wepp;

TEST(TcpListener, Start)
{
    const uint16_t port = 54344;

    {
        Socket::TcpListener listener;
        EXPECT_TRUE(listener.start(port).wait(std::chrono::seconds(3)).successful());
    }
    {
        Socket::TcpListener listener;
        EXPECT_TRUE(listener.start(port).wait(std::chrono::seconds(3)).successful());
        EXPECT_TRUE(listener.stop().wait(std::chrono::seconds(3)).successful());
    }
    {
        Socket::TcpListener listener1, listener2;

        EXPECT_TRUE(listener1.start(port).wait(std::chrono::seconds(3)).successful());
        EXPECT_TRUE(listener2.start(port).wait(std::chrono::seconds(3)).failed());
    }

    {
        Socket::TcpListener listener;
        GTEST_PRINT("Destroying not yet started listener 1.");
    }
    GTEST_PRINT("Destroyed not yet started listener 1.");
    {
        Socket::TcpListener listener;
        GTEST_PRINT("Starting listener 2.");
        EXPECT_NO_THROW(listener.start(port).wait().successful());
        GTEST_PRINT("Destroying started listener 2.");
    }
    GTEST_PRINT("Destroyed started listener 2.");
    {
        Socket::TcpListener listener;
        GTEST_PRINT("Starting listener 3.");
        EXPECT_TRUE(listener.start(port).wait().successful());
        GTEST_PRINT("Destroying started listener 3.");
    }
    GTEST_PRINT("Destroyed started listener 3.");
    {
        Socket::TcpListener listener;
        GTEST_PRINT("Starting listener 4.");
        EXPECT_TRUE(listener.start(port).wait().successful());
        GTEST_PRINT("Stopping listener 4.");
        EXPECT_TRUE(listener.stop().wait().successful());
        GTEST_PRINT("Destroying stopped listener 4.");
    }
    GTEST_PRINT("Destroyed started listener 4.");
    {
        Socket::TcpListener listener;
        GTEST_PRINT("Starting listener 5.");
        EXPECT_TRUE(listener.start(port).wait().successful());
        GTEST_PRINT("Starting listener 5 again.");
        EXPECT_TRUE(listener.start(port).wait().failed());
    }
    GTEST_PRINT("Destroyed started listener 5.");
    {
        Socket::TcpListener listener;
        GTEST_PRINT("Stopping listener 6.");
        EXPECT_TRUE(listener.stop().wait().successful());
        GTEST_PRINT("Stopping listener 6 again.");
        EXPECT_TRUE(listener.stop().wait().successful());
    }
    GTEST_PRINT("Destroyed not started listener 6.");
    {
        Socket::TcpListener listener;
        GTEST_PRINT("Stopping listener 7.");
        EXPECT_TRUE(listener.stop().wait().successful());
        GTEST_PRINT("Starting listener 7.");
        EXPECT_TRUE(listener.start(port).wait().successful());
        GTEST_PRINT("Starting listener 7 again.");
        EXPECT_TRUE(listener.start(port).wait().failed());
        GTEST_PRINT("Stopping listener 7 again.");
        EXPECT_TRUE(listener.stop().wait().successful());
    }
    GTEST_PRINT("Destroyed stopped listener 7.");

}

TEST(TcpListener, Listen)
{
    const uint16_t port = 54344;

    {
        Socket::TcpListener listener;
        EXPECT_TRUE(listener.start(port).wait().successful());

        auto start = std::chrono::high_resolution_clock::now();
        auto task = listener.listen().wait(std::chrono::seconds(1));
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        EXPECT_GE(elapsed.count(), 0.9f);
        EXPECT_LT(elapsed.count(), 1.1f);
        EXPECT_TRUE(task.timeout());
    }

    {
        Socket::TcpListener listener;
        EXPECT_TRUE(listener.start(port).wait().successful());

        auto listenTask = listener.listen();
        EXPECT_TRUE(listenTask.pending());

        Socket::TcpSocket client;
        EXPECT_TRUE(client.connect("127.0.0.1", port));

        EXPECT_TRUE(listenTask.wait(std::chrono::seconds(3)).successful());
        auto server = listenTask();

        const int bufferSize1 = 13;
        const int bufferSize2 = 8;
        char serverSendBuffer[bufferSize1] = "Hello world!";
        char clientRecvBuffer[bufferSize1] = "\0\0\0\0\0\0\0\0\0\0\0\0";
        char clientSendBuffer[bufferSize2] = "foo bar";
        char serverRecvBuffer[bufferSize1] = "\0\0\0\0\0\0\0\0\0\0\0\0";

        EXPECT_EQ(server->send(serverSendBuffer, bufferSize1), bufferSize1);
        EXPECT_EQ(client.receive(clientRecvBuffer, bufferSize1), bufferSize1);
        EXPECT_STREQ(clientRecvBuffer, "Hello world!");

        EXPECT_EQ(client.send(clientSendBuffer, bufferSize2), bufferSize2);
        EXPECT_EQ(server->receive(serverRecvBuffer, bufferSize1), bufferSize2);
        serverRecvBuffer[bufferSize2] = '\0';
        EXPECT_STREQ(serverRecvBuffer, "foo bar");
    }

}

