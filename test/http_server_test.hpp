#include "test.hpp"
#include "wepp/http/server.hpp"
#include "wepp/socket/tcpSocket.hpp"
#include "wepp/semaphore.hpp"
#include <iostream>

using namespace Wepp;

TEST(Http_Server, Start_Stop)
{
    const uint16_t port = 7654;
    {
        Http::Server server;
        GTEST_PRINT("Destroying not started server.");
    }
    GTEST_PRINT("Destroyed not started server.");
    {
        Http::Server server;
        GTEST_PRINT("Starting throw server.");
        EXPECT_NO_THROW(server.start(port));
        GTEST_PRINT("Starting throw server succeeded.");
    }
    GTEST_PRINT("Destroyed server throw.");
    {
        Http::Server server;
        GTEST_PRINT("Starting server 1.");
        EXPECT_TRUE(server.start(port).wait().successful());
        GTEST_PRINT("Starting successful 1.");
    }
    GTEST_PRINT("Destroyed server 1.");
    {
        Http::Server server;
        GTEST_PRINT("Starting server 2.");
        EXPECT_TRUE(server.start(port).wait().successful());
        GTEST_PRINT("Starting successful 2.");
        EXPECT_TRUE(server.stop().wait().successful());
    }
    GTEST_PRINT("Destroyed server 2.");
    {
        Http::Server server;
        GTEST_PRINT("Starting server 3.");
        EXPECT_TRUE(server.start(port).wait().successful());
        GTEST_PRINT("Starting server 3.1.");
        EXPECT_TRUE(server.start(port).wait().failed());
    }
    GTEST_PRINT("Destroyed server 3.");
    {
        Http::Server server;
        GTEST_PRINT("Stopping server 4.");
        EXPECT_TRUE(server.stop().wait().successful());
        GTEST_PRINT("Stopping server 4.1.");
        EXPECT_TRUE(server.stop().wait().successful());
    }
    GTEST_PRINT("Destroyed server 4.");
    {
        Http::Server server;
        GTEST_PRINT("Stopping server 5.");
        EXPECT_TRUE(server.stop().wait().successful());
        GTEST_PRINT("Starting server 5.");
        EXPECT_TRUE(server.start(port).wait().successful());
        GTEST_PRINT("Starting server 5.1.");
        EXPECT_TRUE(server.start(port).wait().failed());
        GTEST_PRINT("Stopping server 5.1.");
        EXPECT_TRUE(server.stop().wait().successful());
    }
    GTEST_PRINT("Destroyed server 5.");
}


TEST(Server, Route)
{
    {
        Http::Server server;
        Semaphore sem1, sem2;

        server.route["GET"]["/foo/bar"] = [&sem1](const Http::Request & request, Http::Response & response)
        {
            EXPECT_STREQ(request.method().c_str(), "GET");
            EXPECT_STREQ(request.resource().c_str(), "/foo/bar");
            EXPECT_STREQ(request.version().c_str(), "HTTP/1.1");
            
            EXPECT_STREQ(request.headers().find("Key 1").c_str(), "value 1");
            EXPECT_STREQ(request.headers().find("Key 2").c_str(), "value 2");
            EXPECT_STREQ(request.headers().find("content-length").c_str(), "8");

            EXPECT_EQ(request.body().size(), size_t(8));
            EXPECT_STREQ(std::string(request.body().data(), request.body().size()).c_str(), "cool kid");


            response << "foo bar response.";
            sem1.notifyOne();
        };

        server.route["POST"]["/hello/world"] = [&sem2](const Http::Request & request, Http::Response & response)
        {
            EXPECT_STREQ(request.method().c_str(), "POST");
            EXPECT_STREQ(request.resource().c_str(), "/hello/world");
            EXPECT_STREQ(request.version().c_str(), "HTTP/1.1");

            EXPECT_STREQ(request.headers().find("Key 3").c_str(), "value 3");
            EXPECT_STREQ(request.headers().find("key 4").c_str(), "value 4");
            EXPECT_STREQ(request.headers().find("content-length").c_str(), "13");
          
            EXPECT_EQ(request.body().size(), size_t(13));
            EXPECT_STREQ(std::string(request.body().data(), request.body().size()).c_str(), "goodbye world");

            response << "hello world response.";
            sem2.notifyOne();
        };

        const uint16_t port = 54343;
        EXPECT_TRUE(server.start(port).wait() == TaskStatus::Successful);

        const size_t recvBufferSize = 16384;
        char recvBuffer[recvBufferSize];

        {
            Socket::TcpSocket peer;
            EXPECT_TRUE(peer.connect("127.0.0.1", port));

            std::string fooBarData =
                          "GET /foo/bar HTTP/1.1\r\n";
            fooBarData += "key 1: value 1\r\n";
            fooBarData += "key 2: value 2\r\n";
            fooBarData += "content-length: 8\r\n\r\n";
            fooBarData += "cool kid";

            EXPECT_EQ(peer.send(fooBarData), int(fooBarData.size()));
            int recvStatus = peer.receive(recvBuffer, recvBufferSize);
            EXPECT_GT(recvStatus, 0);
            recvBuffer[recvStatus + 1] = 0;
        }
        {
            Socket::TcpSocket peer;
            EXPECT_TRUE(peer.connect("127.0.0.1", port));

            std::string helloWorldData =
                              "POST /hello/world HTTP/1.1\r\n";
            helloWorldData += "key 3: value 3\r\n";
            helloWorldData += "key 4: value 4\r\n";
            helloWorldData += "content-length: 13\r\n\r\n";
            helloWorldData += "goodbye world";

            EXPECT_EQ(peer.send(helloWorldData), int(helloWorldData.size()));
            int recvStatus = peer.receive(recvBuffer, recvBufferSize);
            EXPECT_GT(recvStatus, 0);
            recvBuffer[recvStatus + 1] = 0;
        }

        sem1.wait();
        sem2.wait();
    }


}

/*
TEST(Server, Route)
{
    {
        Http::Server server;
        Semaphore sem1, sem2;

        server.route["GET"]["/work/employe/<[\\w ]*>"] = [&sem1](const Http::Request & request, Http::Response & response)
        {
            std::cout << "Received " << request.method() << " request." << std::endl;
            std::cout << "Resource: " << request.resource() << std::endl;;
            std::cout << "Headers(" << request.headers().size() << "): " << std::endl;
            size_t hCount = 0;
            for (auto it = request.headers().begin(); it != request.headers().end(); it++)
            {
                std::cout << "  " << hCount << "  " << it->first << ": " << it->second << std::endl;
                hCount++;
            }
            std::cout << "Body(" << request.body().size() << "): " << std::endl;
            if (request.body().size())
            {
                std::cout << std::string(request.body().data(), request.body().size()) << std::endl;
            }
            
            response << request.resource();

            sem1.notifyOne();
        };

        server.route["GET"]["/work/task/<[0-9]+>"] = [&sem2](const Http::Request &, Http::Response & )
        {
            std::cout << "Requesting task!" << std::endl;
            sem2.notifyOne();
        };

        EXPECT_TRUE(server.start(80).wait() == TaskStatus::Successful);

        sem1.wait();
        sem2.wait();
    }

    EXPECT_TRUE(true);
}
*/