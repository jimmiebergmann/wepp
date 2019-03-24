#include "gtest/gtest.h"
#include "wepp/http/request.hpp"

using namespace Wepp;

TEST(Request, Request)
{
    {
        Http::Request request;

        EXPECT_EQ(request.method(), "");
        EXPECT_EQ(request.resource(), "");
        EXPECT_EQ(request.version(), "HTTP/1.1");
        EXPECT_EQ(request.headers().size(), size_t(0));

        EXPECT_EQ(request.method("test").method(), "TEST");
        EXPECT_EQ(request.method("Post").method(), "POST");
        EXPECT_EQ(request.method("GET").method(), "GET");
        EXPECT_EQ(request.method("").method(), "");

        EXPECT_EQ(request.resource("foo/bar").resource(), "foo/bar");
        EXPECT_EQ(request.resource("test").resource(), "test");
        EXPECT_EQ(request.resource("cool / kid").resource(), "cool / kid");
        EXPECT_EQ(request.resource("/Foo.txt").resource(), "/Foo.txt");

        EXPECT_EQ(request.version("1.1").version(), "1.1");
        EXPECT_EQ(request.version("HTTP/1.1").version(), "HTTP/1.1");
        EXPECT_EQ(request.version("Version: 1").version(), "Version: 1");
        EXPECT_EQ(request.version("").version(), "");
    }
    {
        Http::Request request;
        EXPECT_EQ(request.headers().size(), size_t(0));

        request.headers().insert({ "key1", "value 1" });
        request.headers().insert({ "key2", "value 2" });
        request.headers().insert({ "key3", "value 3" });
        EXPECT_EQ(request.headers().size(), size_t(3));

        auto it1 = request.headers().find("key1");
        auto it2 = request.headers().find("key2");
        auto it3 = request.headers().find("key3");

        EXPECT_NE(it1, request.headers().end());
        EXPECT_NE(it2, request.headers().end());
        EXPECT_NE(it3, request.headers().end());

        EXPECT_EQ(it1->first, "key1");
        EXPECT_EQ(it1->second, "value 1");
        EXPECT_EQ(it2->first, "key2");
        EXPECT_EQ(it2->second, "value 2");
        EXPECT_EQ(it3->first, "key3");
        EXPECT_EQ(it3->second, "value 3");

        {
            const Http::Request & const_req = request;
            EXPECT_EQ(const_req.headers().size(), size_t(3));
        }
    }
}