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

#ifndef WEPP_HTTP_HTTP_RECEIVER_HPP
#define WEPP_HTTP_HTTP_RECEIVER_HPP

#include "wepp/build.hpp"
#include "wepp/http/request.hpp"
#include "wepp/http/response.hpp"
#include "wepp/socket/tcpSocket.hpp"
#include <memory>
#include <functional>

/**
* Wepp namespace.
*
*/
namespace Wepp
{

    /**
    * Http namespace.
    *
    */
    namespace Http
    {

        /**
        * Http receiver class.
        *
        */
        class HttpReceiver
        {

        public:

            /**
            * Default constructor.
            *
            */
            HttpReceiver(const std::shared_ptr<Socket::TcpSocket> socket, Request & request, Response & response, const size_t bufferSize);

            /**
            * Receive and parse data.
            *
            * @param[in] onRequest - Function executed when the request line has been received. The receiver is cancelled if the function returns false.
            * @param[in] onHeaders - Function executed when all headers are received. The receiver is cancelled if the function returns false.
            *
            */
            bool receive(std::function<bool(Request &, Response &)> onRequest, std::function<bool(Request &, Response &)> onHeaders);

        private:

            std::shared_ptr<Socket::TcpSocket> m_socket;     /**< Socket, receiving data from.*/
            Request & m_request;                             /**< Reference to request.*/
            Response & m_response;                           /**< Reference to response.*/
            const size_t m_bufferSize;                       /**< Size of buffers.*/

        };

    }

}

#endif