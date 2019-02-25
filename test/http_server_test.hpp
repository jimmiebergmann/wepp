#include "gtest/gtest.h"
#include "wepp/http/server.hpp"
#include "wepp/semaphore.hpp"
#include <iostream>

using namespace Wepp;

TEST(Server, Start)
{
    {
        Http::Server server;

        auto ret = server.start().wait();
    }


    EXPECT_TRUE(true);
}

/*TEST(Server, Route)
{
    {
        Http::Server server;
        Semaphore sem1, sem2;

        server.route["GET"]["/work/employe/<[\\w ]*>"] = [&sem1](const Http::Request & request, Http::Response & response)
        {
            std::cout << "Requesting employe!" << std::endl;
            sem1.notifyAll();
        };

        server.route["GET"]["/work/task/<[0-9]*>"] = [&sem2](const Http::Request & request, Http::Response & response)
        {
            std::cout << "Requesting task!" << std::endl;
            sem2.notifyAll();
        };

        EXPECT_TRUE(server.start().wait() == TaskStatus::Successful);

        sem1.wait();
        sem2.wait();

    }


    EXPECT_TRUE(true);
}*/