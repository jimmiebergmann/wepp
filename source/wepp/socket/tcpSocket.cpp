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

namespace Wepp
{

    namespace Socket
    {

        TcpSocket::TcpSocket()
        {
           
        }

        TcpSocket::TcpSocket(const Handle handle) :
            Socket(handle)
        { }

        TcpSocket::~TcpSocket()
        {
        }

        TcpSocket & TcpSocket::operator = (const Handle & handle)
        {
            m_handle = handle;
            return *this;
        }

        bool TcpSocket::connect(const std::string & endpoint, const uint16_t port)
        {
            // Get address as integer.
            uint32_t endpointAddr = htonl(INADDR_ANY);
            if (endpoint.size() && inet_pton(AF_INET, endpoint.c_str(), &endpointAddr) != 1)
            {
                //std::cerr << "Invalid ip address: " << address << " - error: " << Socket::getLastError() << std::endl;
                return false;
            }

            // Create listen socket.
            m_handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (WeppIsSocketInvalid(m_handle))
            {
                //std::cerr << "Socket creation failed with error: " << Socket::getLastError() << std::endl;
                m_handle = 0;
                return false;
            }

            // Create host address.
            sockaddr_in host;
            host.sin_family = AF_INET;
            host.sin_addr.s_addr = endpointAddr;
            host.sin_port = htons(port);

            // Connect
            if (::connect(m_handle, reinterpret_cast<const WEPP_SOCKADDR_TYPE *>(&host), sizeof(sockaddr_in)) != 0)
            {
                //std::cerr << "Failed to connect: " << Socket::getLastError() << std::endl;
                WeppCloseSocket(m_handle);
                m_handle = 0;
                return false;
            }

            return true;
        }

        int TcpSocket::receive(char * buffer, const int length)
        {
            return ::recv(m_handle, buffer, length, 0);
        }

        int TcpSocket::send(const char * data, const int length)
        {
            return ::send(m_handle, data, length , 0);
        }

        int TcpSocket::send(const std::string & data)
        {
            return ::send(m_handle, data.c_str(), static_cast<int>(data.size()), 0);
        }

    }

}
