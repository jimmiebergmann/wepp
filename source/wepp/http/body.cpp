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

#include "wepp/http/body.hpp"
#include <algorithm>

namespace Wepp
{

    namespace Http
    {

        Body::Body()
        { }

        char * Body::data()
        {
            if (!m_data.size())
            {
                return nullptr;
            }
            return &m_data[0];
        }

        const char * Body::data() const
        {
            if (!m_data.size())
            {
                return nullptr;
            }
            return &m_data[0];
        }

        size_t Body::size() const
        {
            return m_data.size();
        }

        Body & Body::append(const char * data, const size_t size)
        {
            std::copy(data, data + size, std::back_inserter<std::vector<char> >(m_data));
            return *this;
        }

        Body & Body::clear()
        {
            m_data.clear();
            return *this;
        }

        Body & Body::operator =(const std::string & string)
        {
            m_data.assign(string.begin(), string.end());
            return *this;
        }

        Body & Body::operator <<(const std::string & string)
        {
            std::copy(string.begin(), string.end(), std::back_inserter<std::vector<char> >(m_data));
            return *this;
        }

    }

}