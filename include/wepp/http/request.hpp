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

#ifndef WEPP_HTTP_REQUEST_HPP
#define WEPP_HTTP_REQUEST_HPP

#include "wepp/build.hpp"
#include "wepp/http/body.hpp"
#include <string>
#include <map>

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
        * Http request class.
        *
        */
        class WEPP_API Request
        {

        public:

            /**
            * Header map type.
            *
            */
            typedef std::map<std::string, std::string> HeaderMap;

            /**
            * Default constructor.
            *
            */
            Request();

            const std::string & method() const;

            /**
            * Get resouce.
            *
            */
            const std::string & resource() const;

            /**
            * Get version.
            *
            */
            const std::string & version() const;

            /**
            * Get headers.
            *
            */
            HeaderMap & headers();

            /**
            * Get const headers.
            *
            */
            const HeaderMap & headers() const;

            /**
            * Get body.
            *
            */
            const Body & body() const;

            /**
            * Get const body.
            *
            */
            Body & body();

            /**
            * Set method.
            *
            */
            Request & method(const std::string & method);

            /**
            * Set resource.
            *
            */
            Request & resource(const std::string & resource);

        private:

            std::string m_method;       /**< Request method. */
            std::string m_resource;     /**< Request URI/resource. */
            std::string m_version;      /**< Version of HTTP protocol. */

            HeaderMap m_headers;        /**< Headers map. */
            Body m_body;                /**< Request body. */

        };

    }

}

#endif
