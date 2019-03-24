#include "gtest/gtest.h"
#include "wepp/http/router.hpp"

using namespace Wepp;

TEST(Http_Router, Method)
{
    {
        {
            Http::Router router;
            EXPECT_EQ(router.methodCount(), size_t(0));

            auto & method1 = router[Http::Method::Get];
            EXPECT_EQ(router.methodCount(), size_t(1));
            EXPECT_STREQ(method1.name().c_str(),  "GET");

            auto & method2 = router[Http::Method::Head];
            EXPECT_EQ(router.methodCount(), size_t(2));
            EXPECT_STREQ(method2.name().c_str(),  "HEAD");

            auto & method3 = router[Http::Method::Post];
            EXPECT_EQ(router.methodCount(), size_t(3));
            EXPECT_STREQ(method3.name().c_str(),  "POST");

            auto & method4 = router[Http::Method::Put];
            EXPECT_EQ(router.methodCount(), size_t(4));
            EXPECT_STREQ(method4.name().c_str(),  "PUT");

            auto & method5 = router[Http::Method::Delete];
            EXPECT_EQ(router.methodCount(), size_t(5));
            EXPECT_STREQ(method5.name().c_str(),  "DELETE");

            auto & method6 = router[Http::Method::Trace];
            EXPECT_EQ(router.methodCount(), size_t(6));
            EXPECT_STREQ(method6.name().c_str(),  "TRACE");

            auto & method7 = router[Http::Method::Option];
            EXPECT_EQ(router.methodCount(), size_t(7));
            EXPECT_STREQ(method7.name().c_str(),  "OPTION");

            auto & method8 = router[Http::Method::Connect];
            EXPECT_EQ(router.methodCount(), size_t(8));
            EXPECT_STREQ(method8.name().c_str(),  "CONNECT");

            auto & method9 = router[Http::Method::Patch];
            EXPECT_EQ(router.methodCount(), size_t(9));
            EXPECT_STREQ(method9.name().c_str(),  "PATCH");
        }

        {
            Http::Router router;
            EXPECT_EQ(router.methodCount(), size_t(0));

            size_t count = 0;
            size_t tmp = count;
            for (size_t i = 0; i < 3; i++)
            {
                auto & method1 = router["Get"];
                tmp = std::min<size_t>(++count, size_t(10));
                count = tmp;
                EXPECT_EQ(router.methodCount(), count);
                EXPECT_STREQ(method1.name().c_str(),  "GET");

                auto & method2 = router["hEad"];
                tmp = std::min<size_t>(++count, size_t(10));
                count = tmp;
                EXPECT_EQ(router.methodCount(), count);
                EXPECT_STREQ(method2.name().c_str(),  "HEAD");

                auto & method3 = router["poSt"];
                tmp = std::min<size_t>(++count, size_t(10));
                count = tmp;
                EXPECT_EQ(router.methodCount(), count);
                EXPECT_STREQ(method3.name().c_str(),  "POST");

                auto & method4 = router["puT"];
                tmp = std::min<size_t>(++count, size_t(10));
                count = tmp;
                EXPECT_EQ(router.methodCount(), count);
                EXPECT_STREQ(method4.name().c_str(),  "PUT");

                auto & method5 = router["Delete"];
                tmp = std::min<size_t>(++count, size_t(10));
                count = tmp;
                EXPECT_EQ(router.methodCount(), count);
                EXPECT_STREQ(method5.name().c_str(),  "DELETE");

                auto & method6 = router["tRace"];
                tmp = std::min<size_t>(++count, size_t(10));
                count = tmp;
                EXPECT_EQ(router.methodCount(), count);
                EXPECT_STREQ(method6.name().c_str(),  "TRACE");

                auto & method7 = router["opTion"];
                tmp = std::min<size_t>(++count, size_t(10));
                count = tmp;
                EXPECT_EQ(router.methodCount(), count);
                EXPECT_STREQ(method7.name().c_str(),  "OPTION");

                auto & method8 = router["CONNECT"];
                tmp = std::min<size_t>(++count, size_t(10));
                count = tmp;
                EXPECT_EQ(router.methodCount(), count);
                EXPECT_STREQ(method8.name().c_str(),  "CONNECT");

                auto & method9 = router["PATCH"];
                tmp = std::min<size_t>(++count, size_t(10));
                count = tmp;
                EXPECT_EQ(router.methodCount(), count);
                EXPECT_STREQ(method9.name().c_str(),  "PATCH");

                auto & methodPost10 = router["fubar"];
                tmp = std::min<size_t>(++count, size_t(10));
                count = tmp;
                EXPECT_EQ(router.methodCount(), count);
                EXPECT_STREQ(methodPost10.name().c_str(),  "FUBAR");
            }
        }
    } 
}

TEST(Http_RouteMethod, Path)
{
    {
        Http::Router router;
        EXPECT_NO_THROW(router[Http::Method::Get]["/work/task/123"]);
        EXPECT_NO_THROW(router[Http::Method::Get]["/work/task/123"]);
        router[Http::Method::Get]["/work/task/123"] = [](const Http::Request &, Http::Response &) {};

        std::vector<std::string> matches;
        Http::Router::CallbackFunc func;

        EXPECT_EQ(matches.size(), size_t(0));
        EXPECT_NO_THROW(func = router[Http::Method::Get].find("/work/task/123", matches));
        EXPECT_NE(func, nullptr);
        EXPECT_EQ(matches.size(), size_t(0));

        EXPECT_NO_THROW(func = router[Http::Method::Get].find("/work/task/12foo3", matches));
        EXPECT_EQ(func,nullptr);
        EXPECT_EQ(matches.size(), size_t(0));
    }

    {
        Http::Router router;
        std::string routePath = "/work/task/_<[0/-9]+>_<.*>";
        EXPECT_NO_THROW(router[Http::Method::Get][routePath]);
        router[Http::Method::Get][routePath] = [](const Http::Request &, Http::Response &) {};

        std::vector<std::string> matches;
        Http::Router::CallbackFunc func;

        EXPECT_EQ(matches.size(), size_t(0));
        EXPECT_NO_THROW(func = router[Http::Method::Get].find("/work/task/_123_cool", matches));
        EXPECT_NE(func, nullptr);
        EXPECT_EQ(matches.size(), size_t(2));
        EXPECT_STREQ(matches[0].c_str(), "123");
        EXPECT_STREQ(matches[1].c_str(), "cool");

        EXPECT_NO_THROW(func = router[Http::Method::Get].find("/work/task/12foo3", matches));
        EXPECT_EQ(func,nullptr);
        EXPECT_EQ(matches.size(), size_t(0));
    }

    {
        Http::Router router;
        std::string routePath = "/work/task/_<[0/-9]+\\>>_<.*>";
        EXPECT_NO_THROW(router[Http::Method::Get][routePath]);
        router[Http::Method::Get][routePath] = [](const Http::Request &, Http::Response &) {};

        std::vector<std::string> matches;
        Http::Router::CallbackFunc func;

        EXPECT_EQ(matches.size(), size_t(0));
        EXPECT_NO_THROW(func = router[Http::Method::Get].find("/work/task/_123>_cool", matches));
        EXPECT_NE(func, nullptr);
        EXPECT_EQ(matches.size(), size_t(2));
        EXPECT_STREQ(matches[0].c_str(), "123>");
        EXPECT_STREQ(matches[1].c_str(), "cool");
    }

    {
        Http::Router router;
        std::string routePath = "/work/task/_<>_<>";
        EXPECT_NO_THROW(router[Http::Method::Get][routePath]);
        router[Http::Method::Get][routePath] = [](const Http::Request &, Http::Response &) {};

        std::vector<std::string> matches;
        Http::Router::CallbackFunc func;

        EXPECT_EQ(matches.size(), size_t(0));
        EXPECT_NO_THROW(func = router[Http::Method::Get].find("/work/task/_123>_cool", matches));
        EXPECT_NE(func, nullptr);
        EXPECT_EQ(matches.size(), size_t(2));
        EXPECT_STREQ(matches[0].c_str(), "123>");
        EXPECT_STREQ(matches[1].c_str(),"cool");
    }
   
}