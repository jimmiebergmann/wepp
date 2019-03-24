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

#ifndef WEPP_SOCKET_TCP_SOCKET_HPP
#define WEPP_SOCKET_TCP_SOCKET_HPP

#include "wepp/build.hpp"
#include "wepp/socket/socket.hpp"
#include "wepp/task.hpp"
#include <string>

/**
* Wepp namespace.
*
*/
namespace Wepp
{

    /**
    * Wepp namespace.
    *
    */
    namespace Socket
    {

        /**
        * Tcp socket class.
        *
        */
        class WEPP_API TcpSocket :  public Socket
        {

        public:

            /**
            * Constructor.
            *
            */
            TcpSocket();

            /**
            * Constructor.
            *
            */
            TcpSocket(const Handle handle);

            /**
            * Destructor.
            *
            */
            ~TcpSocket();

            /**
            * Set the native handle.
            *
            */
            TcpSocket & operator = (const Handle & handle);

            /**
            * Connect to host.
            *
            */
            bool connect(const std::string & endpoint, const uint16_t port);

            /**
            * Receive data.
            *
            */
            int receive(char * buffer, const int length);

            /**
            * Send data
            *
            */
            int send(const char * data, const int length);
            int send(const std::string & data);

        };

    }

}

#endif