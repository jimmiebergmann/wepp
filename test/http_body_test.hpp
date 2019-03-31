#include "gtest/gtest.h"
#include "wepp/http/body.hpp"

using namespace Wepp;

TEST(Http_Body, Body)
{
    {
        Http::Body body;
        EXPECT_EQ(body.size(), size_t(0));
        body.append("foo ", 4);
        EXPECT_EQ(body.size(), size_t(4));
        EXPECT_STREQ(std::string(body.data(), body.size()).c_str(), "foo ");

        body << "bar!";
        EXPECT_EQ(body.size(), size_t(8));
        EXPECT_STREQ(std::string(body.data(), body.size()).c_str(), "foo bar!");

        body = "Hello world";
        EXPECT_EQ(body.size(), size_t(11));
        EXPECT_STREQ(std::string(body.data(), body.size()).c_str(), "Hello world");

        const Http::Body & ref = body;
        EXPECT_EQ(ref.size(), size_t(11));

        body.clear();
        EXPECT_EQ(body.size(), size_t(0));

        body = "Hello world";
        EXPECT_EQ(body.size(), size_t(11));
        EXPECT_STREQ(std::string(body.data(), body.size()).c_str(), "Hello world");
    }
}