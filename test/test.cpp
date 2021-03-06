#include "gtest/gtest.h"

#include "uri_test.hpp"
#include "task_test.hpp"
#include "socket_socket_test.hpp"
#include "socket_tcp_socket_listener_test.hpp"
#include "http_method_test.hpp"
#include "http_router_test.hpp"
#include "http_request_test.hpp"
#include "http_response_test.hpp"
#include "priv_threadPool_test.hpp"
#include "priv_httpReceiver_test.hpp"
//#include "http_server_test.hpp"

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
