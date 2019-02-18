#include "gtest/gtest.h"
#include "wepp/http/router.hpp"

using namespace Wepp;

TEST(Router, method)
{
    {
        {
            Http::Router router;
            EXPECT_TRUE(router.methodCount() == 0);

            auto & method1 = router[Http::Method::Get];
            EXPECT_TRUE(router.methodCount() == 1);
            EXPECT_TRUE(method1.name() == "GET");

            auto & method2 = router[Http::Method::Head];
            EXPECT_TRUE(router.methodCount() == 2);
            EXPECT_TRUE(method2.name() == "HEAD");

            auto & method3 = router[Http::Method::Post];
            EXPECT_TRUE(router.methodCount() == 3);
            EXPECT_TRUE(method3.name() == "POST");

            auto & method4 = router[Http::Method::Put];
            EXPECT_TRUE(router.methodCount() == 4);
            EXPECT_TRUE(method4.name() == "PUT");

            auto & method5 = router[Http::Method::Delete];
            EXPECT_TRUE(router.methodCount() == 5);
            EXPECT_TRUE(method5.name() == "DELETE");

            auto & method6 = router[Http::Method::Trace];
            EXPECT_TRUE(router.methodCount() == 6);
            EXPECT_TRUE(method6.name() == "TRACE");

            auto & method7 = router[Http::Method::Option];
            EXPECT_TRUE(router.methodCount() == 7);
            EXPECT_TRUE(method7.name() == "OPTION");

            auto & method8 = router[Http::Method::Connect];
            EXPECT_TRUE(router.methodCount() == 8);
            EXPECT_TRUE(method8.name() == "CONNECT");

            auto & method9 = router[Http::Method::Patch];
            EXPECT_TRUE(router.methodCount() == 9);
            EXPECT_TRUE(method9.name() == "PATCH");
        }

        {
            Http::Router router;
            EXPECT_TRUE(router.methodCount() == 0);

            size_t count = 0;
            for (size_t i = 0; i < 3; i++)
            {
                auto & method1 = router["Get"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(router.methodCount() == count);
                EXPECT_TRUE(method1.name() == "GET");

                auto & method2 = router["hEad"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(router.methodCount() == count);
                EXPECT_TRUE(method2.name() == "HEAD");

                auto & method3 = router["poSt"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(router.methodCount() == count);
                EXPECT_TRUE(method3.name() == "POST");

                auto & method4 = router["puT"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(router.methodCount() == count);
                EXPECT_TRUE(method4.name() == "PUT");

                auto & method5 = router["Delete"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(router.methodCount() == count);
                EXPECT_TRUE(method5.name() == "DELETE");

                auto & method6 = router["tRace"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(router.methodCount() == count);
                EXPECT_TRUE(method6.name() == "TRACE");

                auto & method7 = router["opTion"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(router.methodCount() == count);
                EXPECT_TRUE(method7.name() == "OPTION");

                auto & method8 = router["CONNECT"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(router.methodCount() == count);
                EXPECT_TRUE(method8.name() == "CONNECT");

                auto & method9 = router["PATCH"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(router.methodCount() == count);
                EXPECT_TRUE(method9.name() == "PATCH");

                auto & methodPost10 = router["fubar"];
                count = std::min<size_t>(++count, 10);
                EXPECT_TRUE(router.methodCount() == count);
                EXPECT_TRUE(methodPost10.name() == "FUBAR");
            }
        }
    } 
}

TEST(RouteMethod, path)
{
    {
        Http::Router router;
        EXPECT_NO_THROW(router[Http::Method::Get]["/work/task/123"]);
        EXPECT_NO_THROW(router[Http::Method::Get]["/work/task/123"]);
        router[Http::Method::Get]["/work/task/123"] = [](const Http::Request & request, Http::Response & response) {};

        std::vector<std::string> matches;
        Http::Router::CallbackFunc func;

        EXPECT_TRUE(matches.size() == 0);
        EXPECT_NO_THROW(func = router[Http::Method::Get].find("/work/task/123", matches));
        EXPECT_TRUE(func != nullptr);
        EXPECT_TRUE(matches.size() == 0);

        EXPECT_NO_THROW(func = router[Http::Method::Get].find("/work/task/12foo3", matches));
        EXPECT_TRUE(func == nullptr);
        EXPECT_TRUE(matches.size() == 0);
    }

    {
        Http::Router router;
        std::string routePath = "/work/task/_<[0/-9]+>_<.*>";
        EXPECT_NO_THROW(router[Http::Method::Get][routePath]);
        router[Http::Method::Get][routePath] = [](const Http::Request & request, Http::Response & response) {};

        std::vector<std::string> matches;
        Http::Router::CallbackFunc func;

        EXPECT_TRUE(matches.size() == 0);
        EXPECT_NO_THROW(func = router[Http::Method::Get].find("/work/task/_123_cool", matches));
        EXPECT_TRUE(func != nullptr);
        EXPECT_TRUE(matches.size() == 2);
        EXPECT_TRUE(matches[0] == "123");
        EXPECT_TRUE(matches[1] == "cool");

        EXPECT_NO_THROW(func = router[Http::Method::Get].find("/work/task/12foo3", matches));
        EXPECT_TRUE(func == nullptr);
        EXPECT_TRUE(matches.size() == 0);
    }

    {
        Http::Router router;
        std::string routePath = "/work/task/_<[0/-9]+\\>>_<.*>";
        EXPECT_NO_THROW(router[Http::Method::Get][routePath]);
        router[Http::Method::Get][routePath] = [](const Http::Request & request, Http::Response & response) {};

        std::vector<std::string> matches;
        Http::Router::CallbackFunc func;

        EXPECT_TRUE(matches.size() == 0);
        EXPECT_NO_THROW(func = router[Http::Method::Get].find("/work/task/_123>_cool", matches));
        EXPECT_TRUE(func != nullptr);
        EXPECT_TRUE(matches.size() == 2);
        EXPECT_TRUE(matches[0] == "123>");
        EXPECT_TRUE(matches[1] == "cool");
    }

    {
        Http::Router router;
        std::string routePath = "/work/task/_<>_<>";
        EXPECT_NO_THROW(router[Http::Method::Get][routePath]);
        router[Http::Method::Get][routePath] = [](const Http::Request & request, Http::Response & response) {};

        std::vector<std::string> matches;
        Http::Router::CallbackFunc func;

        EXPECT_TRUE(matches.size() == 0);
        EXPECT_NO_THROW(func = router[Http::Method::Get].find("/work/task/_123>_cool", matches));
        EXPECT_TRUE(func != nullptr);
        EXPECT_TRUE(matches.size() == 2);
        EXPECT_TRUE(matches[0] == "123>");
        EXPECT_TRUE(matches[1] == "cool");
    }
   
}