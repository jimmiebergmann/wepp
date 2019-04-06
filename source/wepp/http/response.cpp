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

#include "wepp/http/response.hpp"

namespace Wepp
{

    namespace Http
    {

        Response::Response() :
            m_status(Status::Ok)
        { }

        Status Response::status() const
        {
            return m_status;
        }

        Response & Response::status(const Status status)
        {
            m_status = status;
            return *this;
        }

        Headers & Response::headers()
        {
            return m_headers;
        }

        const Headers & Response::headers() const
        {
            return m_headers;
        }

        Body & Response::body()
        {
            return m_body;
        }
        
        const Body & Response::body() const
        {
            return m_body;
        }

        Response & Response::operator <<(const std::string & string)
        {
            m_body << string;
            return *this;
        }

        void Response::clear()
        {
            m_status = Status::Ok;
            m_headers.clear();
            m_body.clear();
        }

    }

}