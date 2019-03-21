#include "gtest/gtest.h"
#include "wepp/http/server.hpp"
#include "wepp/semaphore.hpp"
#include <iostream>

using namespace Wepp;

/*TEST(Server, Start)
{
    {
        Http::Server server;

        auto ret = server.start().wait();
    }


    EXPECT_TRUE(true);
}*/

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

            sem1.notifyOne();
        };

        server.route["GET"]["/work/task/<[0-9]+>"] = [&sem2](const Http::Request & request, Http::Response & response)
        {
            std::cout << "Requesting task!" << std::endl;
            sem2.notifyOne();
        };

        EXPECT_TRUE(server.start().wait() == TaskStatus::Successful);

        sem1.wait();
        sem2.wait();
    }

    EXPECT_TRUE(true);
}