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

#include "wepp/priv/httpReceiver.hpp"
#include <regex>
#include <algorithm>
#include <sstream>

namespace Wepp
{

    namespace Priv
    {

        // Http receiver buffer implementation.
        HttpReceiverBuffer::HttpReceiverBuffer(const size_t size) :
            m_bufferSize(size),
            m_buffer(std::unique_ptr<char[]>(new char[size])),
            m_receivedPosition(0),
            m_currentReceivePointer(m_buffer.get()),
            m_bufferEndPointer(m_buffer.get() + size),
            m_currentPointer(m_buffer.get()),
            m_newlinePointer(nullptr),
            m_lastFindnewlinePointer(nullptr),
            m_newlineResult(HttpReceiverBuffer::FindResult::NotFound)
        {
            if (size == 0)
            {
                throw std::runtime_error("Size of HttpReceiverBuffer is 0.");
            }
        }

        void HttpReceiverBuffer::reset()
        {
            m_receivedPosition = 0;
            m_currentReceivePointer = m_buffer.get();
            m_bufferEndPointer = m_buffer.get() + m_bufferSize;
            m_currentPointer = m_buffer.get();
            m_newlinePointer = nullptr;
            m_lastFindnewlinePointer = nullptr;
            m_newlineResult = HttpReceiverBuffer::FindResult::NotFound;
        }

        size_t HttpReceiverBuffer::size() const
        {
            return m_bufferSize;
        }

        size_t HttpReceiverBuffer::unreadBytes()
        {
            return static_cast<size_t>(m_currentReceivePointer - m_currentPointer);
        }

        int HttpReceiverBuffer::receive(Socket::TcpSocket & socket)
        {
            if (!makeSpace(0))
            {
                return -2;
            }

            int result = socket.receive(m_currentReceivePointer, m_bufferSize - m_receivedPosition);

            if (result)
            {
                const size_t recvSize = static_cast<size_t>(result);
                m_currentReceivePointer += recvSize;
                m_receivedPosition += recvSize;
            }

            return result >= 0 ? result : -1;
        }

        bool HttpReceiverBuffer::receive(const std::string & string)
        {
            if(!makeSpace(string.size()))
            {
                return false;
            }

            std::memcpy(m_currentReceivePointer, string.c_str(), string.size());

            m_currentReceivePointer += string.size();
            m_receivedPosition += string.size();

            return true;
        }

        HttpReceiverBuffer::FindResult HttpReceiverBuffer::readLine(std::string & output, const size_t maxLength)
        {
            findNewline(maxLength);
            if (m_newlineResult != FindResult::Found)
            {
                return m_newlineResult;
            }
            if (maxLength && static_cast<size_t>(m_newlinePointer - m_currentPointer) > maxLength)
            {
                return FindResult::ReachedMaxLength;
            }

            output = std::string(m_currentPointer, m_newlinePointer);

            m_currentPointer = m_newlinePointer + 2;
            m_newlinePointer = nullptr;
            m_newlineResult = FindResult::NotFound;

            return FindResult::Found;
        }

        HttpReceiverBuffer::FindResult HttpReceiverBuffer::regexSearch(std::cmatch & matches, const std::regex & regex, const size_t maxLength)
        {
            findNewline(maxLength);
            if (m_newlineResult != FindResult::Found)
            {
                return m_newlineResult;
            }
            if (static_cast<size_t>(m_newlinePointer - m_currentPointer) > maxLength)
            {
                return FindResult::ReachedMaxLength;
            }

            if (std::regex_search(static_cast<const char *>(m_currentPointer), static_cast<const char *>(m_newlinePointer), matches, regex) == false)
            {
                return FindResult::NotFound;
            }

            m_currentPointer = m_newlinePointer + 2;
            m_newlinePointer = nullptr;
            m_newlineResult = FindResult::NotFound;

            return FindResult::Found;
        }

        HttpReceiverBuffer::FindResult HttpReceiverBuffer::readNewline()
        {
            if (static_cast<size_t>(m_bufferEndPointer - m_currentPointer) < 2 ||
                *m_currentPointer != '\r' || *(m_currentPointer + 1) != '\n')
            {
                return FindResult::NotFound;
            }

            m_currentPointer += 2;
            m_newlinePointer = nullptr;
            m_newlineResult = FindResult::NotFound;

            return FindResult::Found;
        }

        void HttpReceiverBuffer::findNewline(const size_t maxLength)
        {
            if (m_newlineResult == FindResult::Found)
            {
                return;
            }

            const bool useMaxLength = maxLength &&
                                      (m_buffer.get() + maxLength <= m_bufferEndPointer) &&
                                      (static_cast<size_t>(m_currentReceivePointer - m_currentPointer) > maxLength);

            char * findFrom = m_lastFindnewlinePointer ? m_lastFindnewlinePointer : m_currentPointer;
            char * findTo = useMaxLength ? m_currentPointer + maxLength : m_currentReceivePointer;

            char * foundPos = std::find(findFrom, findTo, '\r');
            if (foundPos == findTo)
            {
                m_lastFindnewlinePointer = foundPos;
                m_newlineResult = useMaxLength ? FindResult::ReachedMaxLength : FindResult::NotFound;
                return;
            }
            else if(*(foundPos + 1) != '\n')
            {
                m_lastFindnewlinePointer = foundPos;
                m_newlineResult = FindResult::NotFound;
                return;
            }

            m_lastFindnewlinePointer = nullptr;
            m_newlinePointer = foundPos;
            m_newlineResult = FindResult::Found;
        }

        bool HttpReceiverBuffer::makeSpace()
        {
            if (m_currentReceivePointer != m_bufferEndPointer)
            {
                return true;
            }

            const size_t freeSpace = static_cast<size_t>(m_bufferEndPointer - m_currentReceivePointer);
            const size_t moveSpace = static_cast<size_t>(m_currentPointer   - m_buffer.get());
            const size_t availableSpace = freeSpace + moveSpace;
            if (!availableSpace)
            {
                return false;
            }

            std::memcpy(m_buffer.get(), m_currentPointer, static_cast<size_t>(m_currentReceivePointer - m_currentPointer));

            m_receivedPosition -= moveSpace;
            m_currentReceivePointer -= moveSpace;
            m_currentPointer = m_buffer.get();
            if (m_newlinePointer)
            {
                m_newlinePointer -= moveSpace;
            }
            if (m_lastFindnewlinePointer)
            {
                m_lastFindnewlinePointer -= moveSpace;
            }

            return true;
        }

        bool HttpReceiverBuffer::makeSpace(const size_t requiredSpace)
        {
            const size_t freeSpace = static_cast<size_t>(m_bufferEndPointer - m_currentReceivePointer);
            if (freeSpace >= requiredSpace)
            {
                return true;
            }

            const size_t moveSpace = static_cast<size_t>(m_currentPointer - m_buffer.get());
            const size_t availableSpace = freeSpace + moveSpace;
            if (requiredSpace > availableSpace)
            {
                return false;
            }

            std::memcpy(m_buffer.get(), m_currentPointer, static_cast<size_t>(m_currentReceivePointer - m_currentPointer));

            m_receivedPosition -= moveSpace;
            m_currentReceivePointer -= moveSpace;
            m_currentPointer = m_buffer.get();
            if (m_newlinePointer)
            {
                m_newlinePointer -= moveSpace;
            }
            if (m_lastFindnewlinePointer)
            {
                m_lastFindnewlinePointer -= moveSpace;
            }

            return true;
        }


        // Http receiver implementation.
        HttpReceiver::HttpReceiver(const size_t limitRequestLine, const size_t limitHeaderFieldLine,
                                   const size_t limitHeaderFieldCount, const size_t bufferSize) :
            m_limitRequestLine(std::max<size_t>(limitRequestLine, 16) + 2),
            m_limitHeaderFieldLine(std::max<size_t>(limitHeaderFieldLine, 8) + 2),
            m_limitHeaderFieldCount(limitHeaderFieldCount),
            m_bufferSize(std::max<size_t>(bufferSize, std::max<size_t>(m_limitRequestLine * 2, std::max<size_t>(m_limitHeaderFieldLine * 2, 1024)))),
            m_buffer(m_bufferSize)
        { }

        bool HttpReceiver::receive(std::shared_ptr<Socket::TcpSocket> socket,
                                   Http::Request & request, Http::Response & response,
                                   std::function<bool(Http::Request &, Http::Response &)> onRequest,
                                   std::function<bool(Http::Request &, Http::Response &)> onHeaders)
        {
            response.status(Http::Status::Ok);

            enum class State
            {
                RequestLine,
                Headers,
                Body
            };

            State state = State::RequestLine;
            int recvSize = 0;
            size_t contentLength = 0;
            bool foundContentLengthHeader = false;
            size_t headerCount = 0;

            // Make sure to reset the buffer, due to possibility of multiple calls of this function.
            m_buffer.reset();

            // Parse request line.
            while(state == State::RequestLine)
            {
                recvSize = m_buffer.receive(*socket);
                if (recvSize <= 0)
                {
                    if (recvSize == -2)
                    {
                        response.status(Http::Status::InternalServerError);
                    }
                    //auto error = Socket::TcpSocket::getLastError();
                    return false;
                }

                std::cmatch matches;
                static const std::regex findRequestLineRegex(R"(^([^ ]*) (.*) ([^ ]*)$)");
                auto foundRequestLine = m_buffer.regexSearch(matches, findRequestLineRegex, m_limitRequestLine);
                if (foundRequestLine == HttpReceiverBuffer::FindResult::ReachedMaxLength)
                {
                    response.status(Http::Status::UriTooLong);
                    return false;
                }
                else if (foundRequestLine == HttpReceiverBuffer::FindResult::NotFound)
                {
                    continue;
                }

                request.method(std::string(matches[1].first, matches[1].second));
                request.resource(std::string(matches[2].first, matches[2].second));
                std::string version(matches[3].first, matches[3].second);
                if (version != "HTTP/1.1")
                {
                    response.status(Http::Status::HttpVersionNotSupported);
                    return false;
                }

                if (!onRequest(request, response))
                {
                    return false;
                }

                state = State::Headers;
            }

            while (state == State::Headers)
            {
                if (headerCount > m_limitHeaderFieldCount)
                {
                    response.status(Http::Status::BadRequest);
                    return false;
                }

                std::cmatch matches;
                static const std::regex findHeaderLineRegex(R"(^([^\:]*?)(\s*):(?:\s*)?(.*?)(?:\s*)?$)");
                auto foundHeaderLine = m_buffer.regexSearch(matches, findHeaderLineRegex, m_limitRequestLine);
                if (foundHeaderLine == HttpReceiverBuffer::FindResult::ReachedMaxLength)
                {
                    response.status(Http::Status::RequestHeaderFieldsTooLarge);
                    return false;
                }
                else if (foundHeaderLine == HttpReceiverBuffer::FindResult::NotFound)
                {
                    recvSize = m_buffer.receive(*socket);
                    if (recvSize <= 0)
                    {
                        if (recvSize == -2)
                        {
                            response.status(Http::Status::InternalServerError);
                        }
                        //auto error = Socket::TcpSocket::getLastError();
                        return false;
                    }
                    continue;
                }

                // Now allowing space before colon.
                // See: https://tools.ietf.org/html/rfc7230#section-3.2.4
                if (std::string(matches[2].first, matches[2].second).size() != 0)
                {
                    response.status(Http::Status::BadRequest);
                    return false;
                }

                std::string fieldName = std::string(matches[1].first, matches[1].second);
                std::transform(fieldName.begin(), fieldName.end(), fieldName.begin(), [](int c) -> char {return static_cast<char>(::tolower(c)); });
                std::string fieldValue = std::string(matches[3].first, matches[3].second);

                // Parse content-length.
                if (fieldName == "content-length")
                {
                    // Parse value.
                    std::stringstream ss(fieldValue);
                    size_t tempContentLength = 0;
                    ss >> tempContentLength;
                    if (ss.fail())
                    {
                        response.status(Http::Status::BadRequest);
                        return false;
                    }

                    // Do not allow multiple content-length fields with different values.
                    if (foundContentLengthHeader && contentLength != tempContentLength)
                    {
                        response.status(Http::Status::BadRequest);
                        return false;
                    }

                    contentLength = tempContentLength;
                    foundContentLengthHeader = true;
                }
                // Chunked request is not yet supported
                else if (fieldName == "transfer-encoding")
                {
                    response.status(Http::Status::NotAcceptable);
                    return false;
                }

                request.headers()[fieldName] = fieldValue;
                headerCount++;

                if (m_buffer.readNewline() == HttpReceiverBuffer::FindResult::Found)
                {
                    state = State::Body;
                    continue;
                }

            }

            // Parse body if it's present in request.
            // See: https://tools.ietf.org/html/rfc7230#section-3.3.3
            bool bodyIsPresent = contentLength > 0;
            if (bodyIsPresent)
            {
                size_t totalReceived = 0;
                do
                {
                    totalReceived += m_buffer.readAll(request.body());

                    if (totalReceived == contentLength)
                    {
                        break;
                    }

                    recvSize = m_buffer.receive(*socket);
                    if (recvSize <= 0)
                    {
                        if (recvSize == -2)
                        {
                            response.status(Http::Status::InternalServerError);
                        }
                        //auto error = Socket::TcpSocket::getLastError();
                        return false;
                    }

                } while (state == State::Body);
            }

            return true;
        }

    }

}
