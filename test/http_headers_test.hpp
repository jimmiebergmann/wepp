#include "test.hpp"
#include "wepp/http/headers.hpp"

using namespace Wepp;

TEST(Headers, find)
{
   {
        Http::Headers headers;

        auto header = headers.find("key");
        
        header = "value";
        EXPECT_STREQ(header.c_str(), "value");
        header = headers.find("key");
        EXPECT_STREQ(header.c_str(), "");
    }

    {
        Http::Headers headers;

        EXPECT_FALSE(headers.exists("key 1"));
        EXPECT_FALSE(headers.exists("key 2"));
        EXPECT_FALSE(headers.exists("key 3"));
        EXPECT_FALSE(headers.exists("key 4"));
        EXPECT_FALSE(headers.exists("key 5"));


        EXPECT_STREQ(headers.find("key 1").c_str(), "");
        headers.set("key 1", "value");
        EXPECT_TRUE(headers.exists("key 1"));
        EXPECT_STREQ(headers.find("key 1").c_str(), "value");
        headers.set("key 1", "new value");
        EXPECT_STREQ(headers.find("key 1").c_str(), "new value");

        headers.set("key 2", 123);
        EXPECT_STREQ(headers.find("key 2").c_str(), "123");

        headers.set("key 3", -123);
        EXPECT_STREQ(headers.find("key 3").c_str(), "-123");

        headers.set("key 4", 1.25f);
        EXPECT_STREQ(headers.find("key 4").c_str(), "1.25");

        EXPECT_TRUE(headers.exists("key 1"));
        EXPECT_TRUE(headers.exists("key 2"));
        EXPECT_TRUE(headers.exists("key 3"));
        EXPECT_TRUE(headers.exists("key 4"));
        EXPECT_FALSE(headers.exists("key 5"));
        EXPECT_EQ(headers.size(), size_t(4));

        headers.unset("key 3");
        EXPECT_FALSE(headers.exists("key 3"));
        EXPECT_EQ(headers.size(), size_t(3));
    }
    {
        Http::Headers headers;

        EXPECT_STREQ(headers.find("key 1", "def").c_str(), "def");
        EXPECT_STREQ(headers.find("key 1", 123).c_str(), "123");
        EXPECT_STREQ(headers.find("key 1", -123).c_str(), "-123");
        EXPECT_STREQ(headers.find("key 1", 1.25f).c_str(), "1.25");
    }
}
