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

#ifndef WEPP_PRIV_HTTP_RECEIVER_HPP
#define WEPP_PRIV_HTTP_RECEIVER_HPP

#include "wepp/build.hpp"
#include "wepp/http/request.hpp"
#include "wepp/http/response.hpp"
#include "wepp/socket/tcpSocket.hpp"
#include <memory>
#include <functional>
#include <regex>

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
    namespace Priv
    {

        class WEPP_API HttpReceiverBuffer
        {

        public:

            enum class FindResult
            {
                Found,
                NotFound,
                ReachedMaxLength
            };

            HttpReceiverBuffer(const size_t size);

            void reset();

            size_t size() const;

            size_t unreadBytes();

            int receive(Socket::TcpSocket & socket);
            bool receive(const std::string & string);
      
            FindResult readLine(std::string & output, const size_t maxLength = 0);

            FindResult regexSearch(std::cmatch & matches, const std::regex & regex, const size_t maxLength = 0);

            FindResult readNewline();

            template<typename Container>
            size_t readAll(Container & container)
            {
                const size_t available = static_cast<size_t>(m_currentReceivePointer - m_currentPointer);
                if (available)
                {
                    container.append(m_currentPointer, available);
                    m_currentPointer = m_currentReceivePointer;
                }

                return available;
            }

        private:

            void findNewline(const size_t maxLength = 0);

            bool makeSpace();
            bool makeSpace(const size_t requiredSpace);

            const size_t            m_bufferSize;
            std::unique_ptr<char[]> m_buffer;
            size_t                  m_receivedPosition;
            char *                  m_currentReceivePointer;
            const char *            m_bufferEndPointer;
            char *                  m_currentPointer;
            char *                  m_newlinePointer;
            char *                  m_lastFindnewlinePointer;
            FindResult              m_newlineResult;

        };


        /**
        * Http receiver class.
        *
        */
        class WEPP_API HttpReceiver
        {

        public:
            
            /**
            * Default constructor.
            *
            * @param[in] limitRequestLine       - Maximum allowed length of request line. Default: 8192. Clamped: 32 minimum.
            * @param[in] limitHeaderFieldLine   - Maximum allowed length of header line. Default: 8192. Clamped: 3 minimum.
            * @param[in] limitHeaderFieldCount  - Maximum allowed numbers of header fields. Default: 512. 
            * @param[in] bufferSize             - Internal size of buffers. Default: 131072. Clamped: 512 minimum or, value of limitRequestLine or limitHeaderLine.
            *
            */
            HttpReceiver(const size_t limitRequestLine = 8192, const size_t limitHeaderFieldLine = 8192,
                         const size_t limitHeaderFieldCount = 512, const size_t bufferSize = 131072);

            /**
            * Receive and parse data.
            *
            * The receive method internally uses two sets of buffers, used for swapping if one of them gets full.
            *
            * @param[out] request    - Output data of request.
            * @param[out] request    - Output data of response.
            * @param[in] onRequest   - Function executed when the request line has been received. The receiver is cancelled if the function returns false.
            * @param[in] onHeaders   - Function executed when all headers are received. The receiver is cancelled if the function returns false.
            
            *
            */
            bool receive(std::shared_ptr<Socket::TcpSocket> socket,
                         Http::Request & request, Http::Response & response,
                         std::function<bool(Http::Request &, Http::Response &)> onRequest,
                         std::function<bool(Http::Request &, Http::Response &)> onHeaders);

        private:

            const size_t m_limitRequestLine;                 /**< .*/
            const size_t m_limitHeaderFieldLine;             /**< .*/
            const size_t m_limitHeaderFieldCount;            /**< .*/
            const size_t m_bufferSize;                       /**< Size of buffers.*/

            HttpReceiverBuffer m_buffer;

        };

    }

}

#endif