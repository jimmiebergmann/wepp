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

        /**
        * Buffer, custom made for Http data.
        * Internally used by HttpReceiver class.
        *
        */
        class WEPP_API HttpReceiverBuffer
        {

        public:

            /**
            * Enumerator describing reading results of buffer.
            *
            */
            enum class FindResult
            {
                Found,              /**< Successfully readed data from buffer. */
                NewlineNotFound,    /**< No newline were found in available bytes. */
                RegexNotFound,      /**< Newline were found, but given regex did not match. */
                ReachedMaxLength    /**< Given max length is reached. */
            };

            /**
            * Constructor.
            *
            * @param size - Size of buffer.
            *
            */
            HttpReceiverBuffer(const size_t size);

            /**
            * Prepare buffer for new data to read.
            *
            */
            void reset();

            /**
            * Get allocated size of buffer.
            *
            */
            size_t size() const;

            /**
            * Get number of available bytes to read.
            *
            */
            size_t unreadBytes();

            /**
            * Receive data from socket and append to buffer.
            *
            */
            int receive(Socket::TcpSocket & socket);

            /**
            * Receive data from string and append to buffer.
            *
            */
            bool receive(const std::string & string);
      
            /**
            * Read line from buffer. A new line must be represented as "\r\n"
            *
            * @param output[out]    - String containing read bytes.
            * @param maxLength[in]  - Maximum number of bytes to read.
            *
            * @return Result of line reading.
            *
            */
            FindResult readLine(std::string & output, const size_t maxLength = 0);

            FindResult regexSearch(std::cmatch & matches, const std::regex & regex, const size_t maxLength = 0);

            FindResult readNewline();

            /**
            * Read all available bytes.
            *
            * @param container[out] - Container receving all available bytes.
            *
            * @return Number of read bytes.
            *
            */
            template<typename Container>
            size_t readAll(Container & container);

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
            * Enumerator of different return codes of receive().
            *
            */
            enum class Status
            {
                Ok,
                Disconnected,
                PeerError,
                InternalError
            };

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
            * @param[out] response   - Output data of response.
            * @param[in] onRequest   - Function executed when the request line has been received. The receiver is cancelled if the function returns false.
            * @param[in] onHeaders   - Function executed when all headers are received. The receiver is cancelled if the function returns false.
            
            *
            */
            Status receive(std::shared_ptr<Socket::TcpSocket> socket,
                           Http::Request & request, Http::Response & response,
                           std::function<bool(Http::Request &, Http::Response &)> onRequest);

        private:

            const size_t m_limitRequestLine;                 /**< Maximum number of bytes of received request line. */
            const size_t m_limitHeaderFieldLine;             /**< Maximum number of bytes of received header line.*/
            const size_t m_limitHeaderFieldCount;            /**< Maximum number of received headers.*/
            const size_t m_bufferSize;                       /**< Size of buffers.*/

            HttpReceiverBuffer m_buffer;

        };

    }

}

#include "wepp/priv/httpReceiver.inl"

#endif