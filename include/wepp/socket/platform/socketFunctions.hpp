/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/

#ifndef WEPP_SOCKET_PLATFORM_SOCKET_FUNCTIONS_HPP
#define WEPP_SOCKET_PLATFORM_SOCKET_FUNCTIONS_HPP

#include <string>

#if defined(WEPP_PLATFORM_WINDOWS)
    #define WEPP_SOCKADDR_TYPE SOCKADDR
    #define WeppIsSocketValid(socket) (socket != INVALID_SOCKET)
    #define WeppIsSocketInvalid(socket) (socket == INVALID_SOCKET)
    #define WeppCloseSocket(socket) closesocket(socket)
#elif defined(WEPP_PLATFORM_LINUX)
    #define WEPP_SOCKADDR_TYPE sockaddr
    #define WeppIsSocketValid(socket) (socket >= 0)
    #define WeppIsSocketInvalid(socket) (socket < 0)
    #define WeppCloseSocket(socket) shutdown(socket, SHUT_RDWR);close(socket)
#endif

#endif
