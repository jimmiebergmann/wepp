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

        request.headers().set("key1", "value 1");
        request.headers().set("key2", "value 2");
        request.headers().set("key3", "value 3");
        EXPECT_EQ(request.headers().size(), size_t(3));

        EXPECT_TRUE(request.headers().exists("key1"));
        EXPECT_TRUE(request.headers().exists("key2"));
        EXPECT_TRUE(request.headers().exists("key3"));

        auto key1 = request.headers().find("key1");
        auto key2 = request.headers().find("key2");
        auto key3 = request.headers().find("key3");

        EXPECT_STREQ(key1.c_str(), "value 1");
        EXPECT_STREQ(key2.c_str(), "value 2");
        EXPECT_STREQ(key3.c_str(), "value 3");
        

        {
            const Http::Request & const_req = request;
            EXPECT_EQ(const_req.headers().size(), size_t(3));
        }
    }
}