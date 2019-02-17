#include "gtest/gtest.h"
#include <vector>
#include "wepp/uri.hpp"

using namespace Wepp;

static const std::vector<std::string> test_str = {
    "foo://example.com:8042/over/there?name=ferret#nose",
    "urn:example:animal:ferret:nose",
    "https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top",
    "mailto:John.Doe@example.com"
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

TEST(Uri, Parse)
{
    Uri uri;

    //EXPECT_TRUE(uri.parse(test_str[0]));
    
}