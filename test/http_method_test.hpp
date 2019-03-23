#include "gtest/gtest.h"
#include "wepp/http/method.hpp"

using namespace Wepp;

TEST(Http_Method, Name)
{
    EXPECT_STREQ(Http::getMethodAsString(Http::Method::Get).c_str(), "GET");
    EXPECT_STREQ(Http::getMethodAsString(Http::Method::Head).c_str(), "HEAD");
    EXPECT_STREQ(Http::getMethodAsString(Http::Method::Post).c_str(), "POST");
    EXPECT_STREQ(Http::getMethodAsString(Http::Method::Put).c_str(), "PUT");
    EXPECT_STREQ(Http::getMethodAsString(Http::Method::Delete).c_str(), "DELETE");
    EXPECT_STREQ(Http::getMethodAsString(Http::Method::Trace).c_str(), "TRACE");
    EXPECT_STREQ(Http::getMethodAsString(Http::Method::Option).c_str(), "OPTION");
    EXPECT_STREQ(Http::getMethodAsString(Http::Method::Connect).c_str(), "CONNECT");
    EXPECT_STREQ(Http::getMethodAsString(Http::Method::Patch).c_str(), "PATCH");
}