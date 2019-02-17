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

    EXPECT_FALSE(uri.schema.size());
    EXPECT_FALSE(uri.authority.size());
    EXPECT_FALSE(uri.path.size());
    EXPECT_FALSE(uri.query.size());
    EXPECT_FALSE(uri.fragment.size());
    EXPECT_TRUE(uri.isEmpty());

    uri.schema = uri.authority = uri.path = uri.query = uri.fragment = "foobar";
    EXPECT_FALSE(uri.isEmpty());

    uri.clear();
    EXPECT_FALSE(uri.schema.size());
    EXPECT_FALSE(uri.authority.size());
    EXPECT_FALSE(uri.path.size());
    EXPECT_FALSE(uri.query.size());
    EXPECT_FALSE(uri.fragment.size());
    EXPECT_TRUE(uri.isEmpty());
}

TEST(Uri, Parse)
{
    Uri uri;
    // foo://example.com:8042/over/there?name=ferret#nose
    EXPECT_TRUE(uri.parse(test_str[0]));
    EXPECT_TRUE(uri.schema == "foo");
    EXPECT_TRUE(uri.authority == "example.com:8042");
    EXPECT_TRUE(uri.path == "/over/there");
    EXPECT_TRUE(uri.query == "name=ferret");
    EXPECT_TRUE(uri.fragment == "nose");

    // urn:example:animal:ferret:nose
    EXPECT_TRUE(uri.parse(test_str[1]));
    EXPECT_TRUE(uri.schema == "urn");
    EXPECT_TRUE(uri.authority == "");
    EXPECT_TRUE(uri.path == "example:animal:ferret:nose");
    EXPECT_TRUE(uri.query == "");
    EXPECT_TRUE(uri.fragment == "");

    // https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top
    EXPECT_TRUE(uri.parse(test_str[2]));
    EXPECT_TRUE(uri.schema == "https");
    EXPECT_TRUE(uri.authority == "john.doe@www.example.com:123");
    EXPECT_TRUE(uri.path == "/forum/questions/");
    EXPECT_TRUE(uri.query == "tag=networking&order=newest");
    EXPECT_TRUE(uri.fragment == "top");

    // mailto:John.Doe@example.com
    EXPECT_TRUE(uri.parse(test_str[3]));
    EXPECT_TRUE(uri.schema == "mailto");
    EXPECT_TRUE(uri.authority == "");
    EXPECT_TRUE(uri.path == "John.Doe@example.com");
    EXPECT_TRUE(uri.query == "");
    EXPECT_TRUE(uri.fragment == "");

    // "http:this:is@not.the_authority/this/is/the/path?tag=networking&order=newest#top"
    EXPECT_TRUE(uri.parse(test_str[4]));
    EXPECT_TRUE(uri.schema == "http");
    EXPECT_TRUE(uri.authority == "");
    EXPECT_TRUE(uri.path == "this:is@not.the_authority/this/is/the/path");
    EXPECT_TRUE(uri.query == "tag=networking&order=newest");
    EXPECT_TRUE(uri.fragment == "top");
}