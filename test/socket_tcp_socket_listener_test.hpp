#include "gtest/gtest.h"
#include "wepp/socket/tcpListener.hpp"
#include <thread>

using namespace Wepp;

TEST(TcpListener, Start)
{
    const unsigned short port = 54341;
    
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

}

TEST(TcpListener, Listen)
{
    const unsigned short port = 54341;

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

