#include "gtest/gtest.h"
#include <vector>
#include "wepp/uri.hpp"

using namespace Wepp;

static const std::vector<std::string> test_str = {
    "foo://example.com:8042/over/there?name=ferret#nose",
    "urn:example:animal:ferret:nose",
    "https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top",
    "mailto:John.Doe@example.com",
    "http:this:is@not.the_authority/this/is/the/path?tag=networking&order=newest#top"
};
   

TEST(Uri, Throw)
{
    for (size_t i = 0; i < test_str.size(); i++)
    {
        EXPECT_NO_THROW(Uri uri_no_throw(test_str[i]));
        EXPECT_NO_THROW(Uri uri_no_throw(test_str[i].c_str()));
        EXPECT_NO_THROW(Uri uri_no_throw; uri_no_throw.parse(test_str[i]));
        EXPECT_NO_THROW(Uri uri_no_throw; uri_no_throw.parse(test_str[i].c_str()));
    }
}

TEST(Uri, Clear)
{
    Uri uri;

    EXPECT_EQ(uri.schema.size(), size_t(0));
    EXPECT_EQ(uri.authority.size(), size_t(0));
    EXPECT_EQ(uri.path.size(), size_t(0));
    EXPECT_EQ(uri.query.size(), size_t(0));
    EXPECT_EQ(uri.fragment.size(), size_t(0));
    EXPECT_TRUE(uri.isEmpty()), size_t(0);

    uri.schema = uri.authority = uri.path = uri.query = uri.fragment = "foobar";
    EXPECT_FALSE(uri.isEmpty());

    uri.clear();
    EXPECT_EQ(uri.schema.size(), size_t(0));
    EXPECT_EQ(uri.authority.size(), size_t(0));
    EXPECT_EQ(uri.path.size(), size_t(0));
    EXPECT_EQ(uri.query.size(), size_t(0));
    EXPECT_EQ(uri.fragment.size(), size_t(0));
    EXPECT_TRUE(uri.isEmpty());
}

TEST(Uri, Parse)
{
    Uri uri;
    // foo://example.com:8042/over/there?name=ferret#nose
    EXPECT_TRUE(uri.parse(test_str[0]));
    EXPECT_STREQ(uri.schema.c_str(), "foo");
    EXPECT_STREQ(uri.authority.c_str(), "example.com:8042");
    EXPECT_STREQ(uri.path.c_str(), "/over/there");
    EXPECT_STREQ(uri.query.c_str(), "name=ferret");
    EXPECT_STREQ(uri.fragment.c_str(), "nose");

    // urn:example:animal:ferret:nose
    EXPECT_TRUE(uri.parse(test_str[1]));
    EXPECT_STREQ(uri.schema.c_str(), "urn");
    EXPECT_STREQ(uri.authority.c_str(), "");
    EXPECT_STREQ(uri.path.c_str(), "example:animal:ferret:nose");
    EXPECT_STREQ(uri.query.c_str(), "");
    EXPECT_STREQ(uri.fragment.c_str(), "");

    // https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top
    EXPECT_TRUE(uri.parse(test_str[2]));
    EXPECT_STREQ(uri.schema.c_str(), "https");
    EXPECT_STREQ(uri.authority.c_str(), "john.doe@www.example.com:123");
    EXPECT_STREQ(uri.path.c_str(), "/forum/questions/");
    EXPECT_STREQ(uri.query.c_str(), "tag=networking&order=newest");
    EXPECT_STREQ(uri.fragment.c_str(), "top");

    // mailto:John.Doe@example.com
    EXPECT_TRUE(uri.parse(test_str[3]));
    EXPECT_STREQ(uri.schema.c_str(), "mailto");
    EXPECT_STREQ(uri.authority.c_str(), "");
    EXPECT_STREQ(uri.path.c_str(), "John.Doe@example.com");
    EXPECT_STREQ(uri.query.c_str(), "");
    EXPECT_STREQ(uri.fragment.c_str(), "");

    // "http:this:is@not.the_authority/this/is/the/path?tag=networking&order=newest#top"
    EXPECT_TRUE(uri.parse(test_str[4]));
    EXPECT_STREQ(uri.schema.c_str(), "http");
    EXPECT_STREQ(uri.authority.c_str(), "");
    EXPECT_STREQ(uri.path.c_str(), "this:is@not.the_authority/this/is/the/path");
    EXPECT_STREQ(uri.query.c_str(), "tag=networking&order=newest");
    EXPECT_STREQ(uri.fragment.c_str(), "top");
}