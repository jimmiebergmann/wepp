#include "test.hpp"
#include "wepp/http/server.hpp"
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