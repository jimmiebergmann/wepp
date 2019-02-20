#include "gtest/gtest.h"
#include "wepp/http/server.hpp"
#include <iostream>
#include <future>
using namespace Wepp;

TEST(Server, Start)
{
    {
        Http::Server server;

        auto ret = server.start().wait();
    }


    EXPECT_TRUE(true);
}