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

#ifndef WEPP_SOCKET_SOCKET_HPP
#define WEPP_SOCKET_SOCKET_HPP

#include "wepp/build.hpp"
#include "wepp/socket/platform/socketHeaders.hpp"

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
        * Socket base class.
        *
        */
        class WEPP_API Socket
        {

        public:

            /**
            * Enumerator of different socket statuses.
            *
            */
            enum Status
            {
                Error,          /**< Error occured during operation.*/
                Timeout,        /**< Operation timed out.*/
                Successful,     /**< Operation succeeded.*/
            };

            /**
            * Handle type.
            *
            */
        #if defined(WEPP_PLATFORM_WINDOWS)
            typedef SOCKET Handle;
        #elif defined(WEPP_PLATFORM_LINUX)
            typedef int Handle;
        #endif

            /**
            * Get latest native error.
            *
            */
            static int getLastError();

            /**
            * Set latest native error.
            *
            */
            static void setLastError(const int error);

            /**
            * Constructor.
            *
            */
            Socket();

            /**
            * Constructing socket by passing handle.
            * The responsibility of closing the handle is transferred to the socket class.
            *
            */
            Socket(const Handle handle);

            /**
            * Virtual destructor.
            *
            */
            virtual ~Socket();

            /**
            * Close socket.
            * Ongoing connection is being terminated and any listening event is terminated.
            *
            */
            void close();

            /**
            * Get handle.
            *
            */
            Handle getHandle() const;

            /**
            * Set handle. Previous handle is being closed.
            *
            */
            void setHandle(const Handle handle);

            /**
            * Release the handle without closing it.
            * Responsibility of closing the handle is placed on the caller.
            *
            */
            void releaseHandle();

            /**
            * Activate or deactivate socket blocking.
            *
            */
            bool setBlocking(const bool status) const;

            /**
            * Activate or deactivate socket reuse address.
            *
            */
            bool setReuseAddress(const bool status) const;

        protected:

            /**
            * Handle of native socket descriptor.
            *
            */
            Handle m_handle;

        };

    }

}

#endif
