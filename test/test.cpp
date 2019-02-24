#include "gtest/gtest.h"

#include "uri_test.hpp"
//#include "task_test.hpp"
#include "http_method_test.hpp"
//#include "http_router_test.hpp"
#include "http_server_test.hpp"

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
