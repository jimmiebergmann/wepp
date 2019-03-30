#ifndef WEPP_TEST_HPP
#define WEPP_TEST_HPP

#include "wepp/build.hpp"
#include "gtest/gtest.h"
#include <iostream>

#if defined(WEPP_PLATFORM_LINUX)
#define GTEST_PRINT(message) std::cout << "\033[0;32m" << "[          ] " << "\033[0;0m" << message << std::endl
#else
#define GTEST_PRINT(message) std::cout << "[          ] " << message << std::endl
#endif

#endif