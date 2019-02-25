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

#include "wepp/http/httpReceiver.hpp"
#include <regex>
#include <iostream>

namespace Wepp
{

    namespace Http
    {

        template<typename Container>
        static size_t findLineEnd(const Container & container)
        {
            auto posIt = std::find(container.begin(), container.end(), '\r');
            if (posIt != container.end())
            {
                ptrdiff_t pos = posIt - container.begin();
                if (pos >= 0 || pos < container.size() || container[pos + 1] == '\n')
                {
                    return static_cast<size_t>(pos);
                }
            }

            return std::string::npos;
        }

        template<typename Container, typename LastIt>
        static size_t findLineEnd(const Container & container, const LastIt lastIt)
        {
            auto posIt = std::find(container.begin(), lastIt, '\r');
            if (posIt != container.end())
            {
                ptrdiff_t pos = posIt - container.begin();
                if (pos >= 0 || pos < container.size() || container[pos + 1] == '\n')
                {
                    return static_cast<size_t>(pos);
                }
            }

            return std::string::npos;
        }

        HttpReceiver::HttpReceiver(const std::shared_ptr<Socket::TcpSocket> socket, Request & request, Response & response, const size_t bufferSize) :
            m_socket(socket),
            m_request(request),
            m_response(response),
            m_bufferSize(bufferSize)
        { }

        bool HttpReceiver::receive(std::function<bool(Request &, Response &)> onRequest, std::function<bool(Request &, Response &)> onHeaders)
        {
            enum class State
            {
                RequestLine,
                Headers,
                Body
            };
            State state = State::RequestLine;

            std::vector<char> swapBuffers[2]{ std::vector<char>(),
                                              std::vector<char>() };
            swapBuffers[0].resize(m_bufferSize + 1);
            swapBuffers[1].resize(m_bufferSize + 1);
            std::vector<char> * buffer = &swapBuffers[0];
            char * bufferRaw = &swapBuffers[0][0];
            size_t bufferPos = 0;
            int recvSize = 0;

            // Parse request line.
            while(state == State::RequestLine)
            {
                if (bufferPos >= m_bufferSize)
                {
                    m_response.status(Status::BadRequest);
                    return false;
                }

                recvSize = m_socket->receive(bufferRaw + bufferPos, m_bufferSize - bufferPos);
                if (recvSize <= 0 || recvSize > m_bufferSize)
                {
                    return false;
                }
                bufferPos += static_cast<size_t>(recvSize);
                
                auto requestLineEnd = findLineEnd(*buffer, buffer->cbegin() + bufferPos);
                if (requestLineEnd != std::string::npos)
                {
                    std::match_results<std::vector<char>::iterator> matches;
                    static const std::regex findRequestRegex(R"(^([^ ]*) (.*) ([^ ]*)$)");
                    if (requestLineEnd == std::string::npos ||
                    std::regex_search(buffer->begin(), buffer->begin() + requestLineEnd, matches, findRequestRegex) == false)
                    {
                        m_response.status(Status::BadRequest);
                        return false;
                    }

                    m_request.method(std::string(matches[1].first, matches[1].second));
                    m_request.resource(std::string(matches[2].first, matches[2].second));
                    std::string version(matches[3].first, matches[3].second);
                    if (version != "HTTP/1.1")
                    {
                        m_response.status(Status::HttpVersionNotSupported);
                        return false;
                    }

                    onRequest(m_request, m_response);

                    state = State::Headers;
                    break;
                }
            }

            // Parse headers.
            

            return true;
        }
    }

}