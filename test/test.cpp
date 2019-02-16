#include <iostream>
#include "wepp/uri.hpp"

using namespace Wepp;

int main()
{
    Uri uri("http://api.test.com/comment?id=4");

    std::cout << "Running test." << std::endl;

    return 0;
}