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

#include "wepp/socket/socket.hpp"
#include "wepp/socket/platform/socketFunctions.hpp"
#include <iostream>


namespace Wepp
{

    namespace Socket
    {

        int Socket::getLastError()
        {
        #if defined(WEPP_PLATFORM_WINDOWS)
            return ::WSAGetLastError();
        #elif defined(WEPP_PLATFORM_LINUX)
            return errno;
        #endif
        }

        void Socket::setLastError(const int error)
        {
        #if defined(WEPP_PLATFORM_WINDOWS)
            ::WSASetLastError(error);
        #elif defined(WEPP_PLATFORM_LINUX)
            errno = error;
        #endif
        }

        Socket::Socket() :
            m_handle(0)
        { }

        Socket::Socket(const Handle handle) :
            m_handle(handle)
        { }

        Socket::~Socket()
        {
            if (m_handle)
            {
                WeppCloseSocket(m_handle);
            }
        }

        const Socket::Handle & Socket::handle() const
        {
            return m_handle;
        }

        bool Socket::setBlocking(const bool status) const
        {
            if( m_handle )
            {
                #if defined(WEPP_PLATFORM_WINDOWS)

                    u_long blocking = status ? 0 : 1;

                    if (ioctlsocket(m_handle, FIONBIO, &blocking) != 0)
                    {
                        //std::cerr << "Setting blocking failed with error: " << Socket::getLastError() << std::endl;
                        return false;
                    }

                    return true;

                #elif defined(WEPP_PLATFORM_LINUX)

                    // Get the current  file descriptor.
                    int opts = fcntl( m_handle, F_GETFL );
                    if (opts < 0)
                    {
                        //std::cerr << "Getting file descriptor failed with error: " << Socket::getLastError() << std::endl;
                        return false;
                    }

                    // Toggle the non blocking flag.
                    opts ^= O_NONBLOCK;

                    // Try to set the new file descriptor.
                    int result = 0;
                    if( ( result = fcntl( m_handle, F_SETFL, opts ) ) < 0 )
                    {
                        //std::cerr << "Setting blocking failed with error: " << Socket::getLastError() << std::endl;
                        return false;
                    }

                    return true;


                #endif
            }

            return false;
        }

        bool Socket::setReuseAddress(const bool status) const
        {
            if (m_handle)
            {
                #if defined(WEPP_PLATFORM_WINDOWS)

                    char enable = status ? 1 : 0;
                    if (setsockopt(m_handle, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(char)) != 0)
                    {
                        return false;
                    }
                    return true;

                #elif defined(WEPP_PLATFORM_LINUX)

                    int enable = status ? 1 : 0;
                    if (setsockopt(m_handle, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
                    {
                        return false;
                    }
                    return true;

                #endif

            }

            return false;
        }


        Socket & Socket::operator = (const Handle & handle)
        {
            m_handle = handle;
            return *this;
        }




        // Intiialize winsock.
        #if defined(WEPP_PLATFORM_WINDOWS)
        struct WinsockInitializer
        {
            WinsockInitializer()
            {
                WSADATA wsaData;
                if (WSAStartup(MAKEWORD(2, 2), &wsaData))
                {
                    std::cerr << "Failed to initialize winsock, version 2.2.";
                }
            }

            ~WinsockInitializer()
            {
                WSACleanup();
            }
        };

        // Create an instance of the intiialize
        static WinsockInitializer g_WinsockInitializer;
        #endif
    }

}
