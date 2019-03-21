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

#include "wepp/http/request.hpp"

namespace Wepp
{

    namespace Http
    {

        Request::Request() :
            m_method(""),
            m_resource(""),
            m_version("HTTP/1.1")
        { }

        const std::string & Request::method() const
        {
            return m_method;
        }

        const std::string & Request::resource() const
        {
            return m_resource;
        }

        const std::string & Request::version() const
        {
            return m_version;
        }

        Request::HeaderMap & Request::headers()
        {
            return m_headers;
        }

        const Request::HeaderMap & Request::headers() const
        {
            return m_headers;
        }

        const Body & Request::body() const
        {
            return m_body;
        }

        Body & Request::body()
        {
            return m_body;
        }

        Request & Request::method(const std::string & method)
        {
            m_method = method;
            std::transform(m_method.begin(), m_method.end(), m_method.begin(), [](int c) -> char {return static_cast<char>(::toupper(c)); });
            return *this;
        }

        Request & Request::resource(const std::string & resource)
        {
            m_resource = resource;
            return *this;
        }

    }

}