#include <iostream>
#include "wepp/uri.hpp"
#include "gtest/gtest.h"

using namespace Wepp;
/*
int main()
{
    Uri uri("http://api.test.com/comment?id=4");

    std::cout << "Running test." << std::endl;

    return 0;
}
*/

TEST(Property, Value)
{
    EXPECT_TRUE(true);
}

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
