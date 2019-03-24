#include "gtest/gtest.h"
#include "wepp/http/response.hpp"

using namespace Wepp;

TEST(Response, Response)
{
    {
        Http::Response response;
        EXPECT_EQ(response.status(), Http::Status::Ok);

        response.status(Http::Status::NotFound);
        EXPECT_EQ(response.status(), Http::Status::NotFound);

        response.status(Http::Status::Conflict).status(Http::Status::Accepted);
        EXPECT_EQ(response.status(), Http::Status::Accepted);
    }
}