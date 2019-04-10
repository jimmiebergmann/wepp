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
#include <string>
#include <chrono>
#include <limits>

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
            * Constructing TCP socket by passing handle.
            * The responsibility of closing the handle is transferred to the socket class.
            *
            */
            TcpSocket(const Handle handle);

            /**
            * Destructor.
            *
            */
            ~TcpSocket();

            /**
            * Creates socket and connects to host.
            *
            */
            Status connect(const std::string & endpoint, const uint16_t port,
                         const std::chrono::duration<double> & timeout = std::chrono::duration<double>::max());

            /**
            * Creates socket, binds to address and listen for incoming connections.
            *
            */
            Status listen(const uint16_t port, const std::string & endpoint, const uint32_t backlog = std::numeric_limits<uint32_t>::max());

            /**
            * Listen for incoming connections.
            *
            * @param peerSocket[out] - Accepted peer socket. 
            *
            * @return Status of peer acception. Status::Successful if accepted, Status::Timeout if timeout is reached, or Status::Error if any error occured.
            *         Check getLastError if Status::Error is returned.
            *
            */
            Status accept(TcpSocket & peerSocket, const std::chrono::duration<double> & timeout = std::chrono::duration<double>::max());

            /**
            * Receive data.
            *
            */
            Socket::Status receive(char * buffer, const int length, int & received, const std::chrono::duration<double> & timeout = std::chrono::duration<double>::max());

            /**
            * Send data
            *
            */
            int send(const char * data, const int length/*, const std::chrono::duration<double> & timeout = std::chrono::duration<double>::max()*/);
            int send(const std::string & data/*, const std::chrono::duration<double> & timeout = std::chrono::duration<double>::max()*/);

        };

    }

}

#endif