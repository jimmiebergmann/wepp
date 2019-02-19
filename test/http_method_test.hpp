#include "gtest/gtest.h"
#include "wepp/http/method.hpp"

using namespace Wepp;

TEST(Method, Name)
{
    EXPECT_TRUE(Http::getMethodName(Http::Method::Get) == "GET");
    EXPECT_TRUE(Http::getMethodName(Http::Method::Head) == "HEAD");
    EXPECT_TRUE(Http::getMethodName(Http::Method::Post) == "POST");
    EXPECT_TRUE(Http::getMethodName(Http::Method::Put) == "PUT");
    EXPECT_TRUE(Http::getMethodName(Http::Method::Delete) == "DELETE");
    EXPECT_TRUE(Http::getMethodName(Http::Method::Trace) == "TRACE");
    EXPECT_TRUE(Http::getMethodName(Http::Method::Option) == "OPTION");
    EXPECT_TRUE(Http::getMethodName(Http::Method::Connect) == "CONNECT");
    EXPECT_TRUE(Http::getMethodName(Http::Method::Patch) == "PATCH");    
}