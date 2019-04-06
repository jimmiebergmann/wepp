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

#include "wepp/http/headers.hpp"

namespace Wepp
{

    namespace Http
    {

        // headers implementations.
        Headers::Headers()
        { }

        size_t Headers::size() const
        {
            return m_headers.size();
        }

        std::map<std::string, std::string>::const_iterator Headers::begin() const
        {
            return m_headers.begin();
        }

        std::map<std::string, std::string>::const_iterator Headers::end() const
        {
            return m_headers.end();
        }

        bool Headers::exists(const std::string & key) const
        {
            return m_headers.find(key) != m_headers.end();
        }

        Headers & Headers::unset(const std::string & key)
        {
            m_headers.erase(key);
            return *this;
        }

        std::string Headers::find(const std::string & key) const
        { 
            std::string lowerKey;
            lowerKey.resize(key.size(), 0);
            std::transform(key.begin(), key.end(), lowerKey.begin(), [](int c) -> char {return static_cast<char>(::tolower(c)); });

            auto it = m_headers.find(lowerKey);
            if (it == m_headers.end())
            {
                return "";
            }

            return it->second;
        }

        Headers & Headers::clear()
        {
            m_headers.clear();
            return *this;
        }

    }

}