#include "gtest/gtest.h"
#include "wepp/http/route.hpp"

using namespace Wepp;

TEST(Route, method)
{
    {
        {
            Http::Route route;
            EXPECT_TRUE(route.methodCount() == 0);

            auto & method1 = route[Http::Method::Get];
            EXPECT_TRUE(route.methodCount() == 1);
            EXPECT_TRUE(method1.name() == "GET");

            auto & method2 = route[Http::Method::Head];
            EXPECT_TRUE(route.methodCount() == 2);
            EXPECT_TRUE(method2.name() == "HEAD");

            auto & method3 = route[Http::Method::Post];
            EXPECT_TRUE(route.methodCount() == 3);
            EXPECT_TRUE(method3.name() == "POST");

            auto & method4 = route[Http::Method::Put];
            EXPECT_TRUE(route.methodCount() == 4);
            EXPECT_TRUE(method4.name() == "PUT");

            auto & method5 = route[Http::Method::Delete];
            EXPECT_TRUE(route.methodCount() == 5);
            EXPECT_TRUE(method5.name() == "DELETE");

            auto & method6 = route[Http::Method::Trace];
            EXPECT_TRUE(route.methodCount() == 6);
            EXPECT_TRUE(method6.name() == "TRACE");

            auto & method7 = route[Http::Method::Option];
            EXPECT_TRUE(route.methodCount() == 7);
            EXPECT_TRUE(method7.name() == "OPTION");

            auto & method8 = route[Http::Method::Connect];
            EXPECT_TRUE(route.methodCount() == 8);
            EXPECT_TRUE(method8.name() == "CONNECT");

            auto & method9 = route[Http::Method::Patch];
            EXPECT_TRUE(route.methodCount() == 9);
            EXPECT_TRUE(method9.name() == "PATCH");
        }

        {
            Http::Route route;
            EXPECT_TRUE(route.methodCount() == 0);

            size_t count = 0;
            for (size_t i = 0; i < 3; i++)
            {
                auto & method1 = route["Get"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(route.methodCount() == count);
                EXPECT_TRUE(method1.name() == "GET");

                auto & method2 = route["hEad"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(route.methodCount() == count);
                EXPECT_TRUE(method2.name() == "HEAD");

                auto & method3 = route["poSt"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(route.methodCount() == count);
                EXPECT_TRUE(method3.name() == "POST");

                auto & method4 = route["puT"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(route.methodCount() == count);
                EXPECT_TRUE(method4.name() == "PUT");

                auto & method5 = route["Delete"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(route.methodCount() == count);
                EXPECT_TRUE(method5.name() == "DELETE");

                auto & method6 = route["tRace"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(route.methodCount() == count);
                EXPECT_TRUE(method6.name() == "TRACE");

                auto & method7 = route["opTion"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(route.methodCount() == count);
                EXPECT_TRUE(method7.name() == "OPTION");

                auto & method8 = route["CONNECT"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(route.methodCount() == count);
                EXPECT_TRUE(method8.name() == "CONNECT");

                auto & method9 = route["PATCH"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(route.methodCount() == count);
                EXPECT_TRUE(method9.name() == "PATCH");

                auto & methodPost10 = route["fubar"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(route.methodCount() == count);
                EXPECT_TRUE(methodPost10.name() == "FUBAR");
            }
        }
    } 
}

TEST(RouteMethod, path)
{

}