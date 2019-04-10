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

#include "wepp/socket/tcpSocket.hpp"
#include "wepp/socket/platform/socketFunctions.hpp"
#include <iostream>

namespace Wepp
{

    namespace Socket
    {

        // Static helper functions
        template<typename T>
        static timeval DurationToTimeval(const T & duration, bool & infinite)
        {
            timeval time{ 0, 0 };

            if (duration > T(std::numeric_limits<long>::max() / 2))
            {
                infinite = true;
                return time;
            }
            if (duration <= T(0))
            {
                infinite = false;
                return time;
            }

            std::chrono::seconds secs = std::chrono::duration_cast<std::chrono::seconds>(duration);
            time.tv_sec = static_cast<long>(secs.count());
            std::chrono::microseconds micros = std::chrono::duration_cast<std::chrono::microseconds>(duration);
            std::chrono::microseconds microPart = micros - std::chrono::microseconds(secs.count() * 1000000);
            time.tv_usec = static_cast<long>(microPart.count());

            infinite = false;
            return time;
        }

       
        // TcpSocket implementations.
        TcpSocket::TcpSocket()
        { }

        TcpSocket::TcpSocket(const Handle handle) :
            Socket(handle)
        { }

        TcpSocket::~TcpSocket()
        { }

        Socket::Status TcpSocket::connect(const std::string & endpoint, const uint16_t port, const std::chrono::duration<double> & timeout)
        {
            close();

            // Create listen socket.
            m_handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (WeppIsSocketInvalid(m_handle))
            {
                //std::cerr << "Socket creation failed with error: " << Socket::getLastError() << std::endl;
                m_handle = 0;
                return Socket::Status::Error;
            }

            // Get address as integer.
            uint32_t endpointAddr = htonl(INADDR_ANY);
            if (endpoint.size() && inet_pton(AF_INET, endpoint.c_str(), &endpointAddr) != 1)
            {
                //std::cerr << "Invalid ip address: " << address << " - error: " << Socket::getLastError() << std::endl;
                close();
                return Socket::Status::Error;
            }

            // Create host address.
            sockaddr_in host;
            host.sin_family = AF_INET;
            host.sin_addr.s_addr = endpointAddr;
            host.sin_port = htons(port);

            setBlocking(false);

            // Create FD
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(m_handle, &fds);
            int nfds = static_cast<int>(m_handle);

            // Wait for connection.
            bool timeoutInfinite = false;
            timeval timeoutData = DurationToTimeval(timeout, timeoutInfinite);
            timeval * pTimeoutData = timeoutInfinite ? NULL : &timeoutData;

            ::connect(m_handle, reinterpret_cast<const WEPP_SOCKADDR_TYPE *>(&host), sizeof(sockaddr_in));

            int sel = ::select(nfds + 1, &fds, &fds, &fds, pTimeoutData);

            // Check status of select.
            try
            {
                if (sel)
                {
                    int result = 0;
                    socklen_t result_len = sizeof(result);
                    if (getsockopt(m_handle, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&result), &result_len) < 0)
                    {
                        throw Socket::Status::Error;
                    }
                    if (result != 0)
                    {
                        throw Socket::Status::Error;
                    }

                    // Successful connection!
                    return Socket::Status::Successful;
                }
                else if (sel == 0)
                {
                    throw Socket::Status::Timeout;
                }
            }
            catch (const Socket::Status & status)
            {
                close();
                return status;
            }

            return Socket::Status::Error;
        }

        Socket::Status TcpSocket::listen(const uint16_t port, const std::string & endpoint, const uint32_t backlog)
        {
            close();

            // Create listen socket.
            m_handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (WeppIsSocketInvalid(m_handle))
            {
                //std::cerr << "Socket creation failed with error: " << Socket::getLastError() << std::endl;
                m_handle = 0;
                return Socket::Status::Error;
            }

            // Get address as integer.
            uint32_t endpointAddr = htonl(INADDR_ANY);
            if (endpoint.size() && inet_pton(AF_INET, endpoint.c_str(), &endpointAddr) != 1)
            {
                //std::cerr << "Invalid ip address: " << address << " - error: " << Socket::getLastError() << std::endl;
                close();
                return Socket::Status::Error;
            }

            // Create host address.
            sockaddr_in service;
            service.sin_family = AF_INET;
            service.sin_addr.s_addr = endpointAddr;
            service.sin_port = htons(port);

            if (::bind(m_handle, reinterpret_cast<const WEPP_SOCKADDR_TYPE *>(&service), sizeof(service)) != 0)
            {
                std::cerr << "Bind function failed with error: " << Socket::getLastError() << std::endl;
                close();
                return Socket::Status::Error;
            }

            int clampedBacklog = (backlog == std::numeric_limits<uint32_t>::max() || backlog > static_cast<uint32_t>(SOMAXCONN)) ? SOMAXCONN : static_cast<int>(backlog);
            if (::listen(m_handle, clampedBacklog) != 0)
            {
                //std::cerr << "Listen function failed with error: " << Socket::getLastError() << std::endl;
                close();
                return Socket::Status::Error;
            }

            setBlocking(false);

            return Socket::Status::Successful;
        }

        Socket::Status TcpSocket::accept(TcpSocket & peerSocket, const std::chrono::duration<double> & timeout)
        {
            if (!m_handle)
            {
                return Socket::Status::Error;
            }

            // Create FD
            fd_set readfd;
            FD_ZERO(&readfd);
            FD_SET(m_handle, &readfd);
            int nfds = static_cast<int>(m_handle);

            // Wait for anything to happend to socket.
            bool timeoutInfinite = false;
            timeval timeoutData = DurationToTimeval(timeout, timeoutInfinite);
            timeval * pTimeoutData = timeoutInfinite ? NULL : &timeoutData;

            int sel = ::select(nfds + 1, &readfd, NULL, NULL, pTimeoutData);

            // Check status of select.
            if (sel == 1)
            {
                // Accept the connection.
                Socket::Handle connection = ::accept(m_handle, NULL, NULL);
                if (WeppIsSocketInvalid(connection))
                {
                    //std::cerr << "accept failed with error: " << Socket::getLastError() << std::endl;
                    return Socket::Status::Error;
                }

                peerSocket.setHandle(connection);
                return Socket::Status::Successful;
            }
            else if (sel == 0)
            {
                return Socket::Status::Timeout;
            }

            return Socket::Status::Error;
        }

        Socket::Status TcpSocket::receive(char * buffer, const int length, int & received, const std::chrono::duration<double> & timeout)
        {
            // Create FD
            fd_set readfd;
            FD_ZERO(&readfd);
            FD_SET(m_handle, &readfd);
            int nfds = static_cast<int>(m_handle);

            // Wait for anything to happend to socket.
            bool timeoutInfinite = false;
            timeval timeoutData = DurationToTimeval(timeout, timeoutInfinite);
            timeval * pTimeoutData = timeoutInfinite ? NULL : &timeoutData;

            int sel = ::select(nfds + 1, &readfd, NULL, NULL, pTimeoutData);

            // Check status of select.
            if (sel == 1)
            {
                received = ::recv(m_handle, buffer, length, 0);
                if (received < 0)
                {
                    return Socket::Status::Error;
                }
                return Socket::Status::Successful;
            }
            else if (sel == 0)
            {
                return Socket::Status::Timeout;
            }

            return Socket::Status::Error;
        }

        int TcpSocket::send(const char * data, const int length/*, const std::chrono::duration<double> & timeout*/)
        {
            return ::send(m_handle, data, length , 0);
        }

        int TcpSocket::send(const std::string & data/*, const std::chrono::duration<double> & timeout*/)
        {
            return ::send(m_handle, data.c_str(), static_cast<int>(data.size()), 0);
        }

    }

}
