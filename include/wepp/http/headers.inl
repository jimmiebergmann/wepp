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

namespace Wepp
{

    namespace Http
    {
        
        template<>
        inline Headers & Headers::set(const std::string & key, const std::string & value)
        {
            std::string lowerKey;
            lowerKey.resize(key.size(), 0);
            std::transform(key.begin(), key.end(), lowerKey.begin(), [](int c) -> char {return static_cast<char>(::tolower(c)); });

            auto it = m_headers.find(lowerKey);
            if (it == m_headers.end())
            {
                m_headers.insert({ lowerKey, value });
            }
            else
            {
                it->second = value;
            }

            return *this;
        }

        template<>
        inline Headers & Headers::set(const std::string & key, const float & value)
        {
            std::string lowerKey;
            lowerKey.resize(key.size(), 0);
            std::transform(key.begin(), key.end(), lowerKey.begin(), [](int c) -> char {return static_cast<char>(::tolower(c)); });

            std::stringstream ss;
            ss << value;

            auto it = m_headers.find(lowerKey);
            if (it == m_headers.end())
            {
                m_headers.insert({ lowerKey, ss.str() });
            }
            else
            {
                it->second = ss.str();
            }

            return *this;
        }

        template<>
        inline Headers & Headers::set(const std::string & key, const double & value)
        {
            std::string lowerKey;
            lowerKey.resize(key.size(), 0);
            std::transform(key.begin(), key.end(), lowerKey.begin(), [](int c) -> char {return static_cast<char>(::tolower(c)); });

            std::stringstream ss;
            ss << value;

            auto it = m_headers.find(lowerKey);
            if (it == m_headers.end())
            {
                m_headers.insert({ lowerKey, ss.str() });
            }
            else
            {
                it->second = ss.str();
            }

            return *this;
        }

        template<typename T>
        inline Headers & Headers::set(const std::string & key, const T & value)
        {
            std::string lowerKey;
            lowerKey.resize(key.size(), 0);
            std::transform(key.begin(), key.end(), lowerKey.begin(), [](int c) -> char {return static_cast<char>(::tolower(c)); });

            auto it = m_headers.find(lowerKey);
            if (it == m_headers.end())
            {
                m_headers.insert({ lowerKey, std::to_string(value) });
            }
            else
            {
                it->second = std::to_string(value);
            }

            return *this;
        }

        template<>
        inline Headers & Headers::set(const std::string & key, const char * value)
        {
            std::string lowerKey;
            lowerKey.resize(key.size(), 0);
            std::transform(key.begin(), key.end(), lowerKey.begin(), [](int c) -> char {return static_cast<char>(::tolower(c)); });

            auto it = m_headers.find(lowerKey);
            if (it == m_headers.end())
            {
                m_headers.insert({ lowerKey, value });
            }
            else
            {
                it->second = value;
            }

            return *this;
        }


        template<>
        inline std::string Headers::find(const std::string & key, const std::string & defaultValue) const
        {
            std::string lowerKey;
            lowerKey.resize(key.size(), 0);
            std::transform(key.begin(), key.end(), lowerKey.begin(), [](int c) -> char {return static_cast<char>(::tolower(c)); });

            auto it = m_headers.find(lowerKey);
            if (it == m_headers.end())
            {
                return defaultValue;
            }

            return it->second;
        }

        template<>
        inline std::string Headers::find(const std::string & key, const float & defaultValue) const
        {
            std::string lowerKey;
            lowerKey.resize(key.size(), 0);
            std::transform(key.begin(), key.end(), lowerKey.begin(), [](int c) -> char {return static_cast<char>(::tolower(c)); });

            auto it = m_headers.find(lowerKey);
            if (it == m_headers.end())
            {
                std::stringstream ss;
                ss << defaultValue;
                return ss.str();
            }

            return it->second;
        }

        template<>
        inline std::string Headers::find(const std::string & key, const double & defaultValue) const
        {
            std::string lowerKey;
            lowerKey.resize(key.size(), 0);
            std::transform(key.begin(), key.end(), lowerKey.begin(), [](int c) -> char {return static_cast<char>(::tolower(c)); });

            auto it = m_headers.find(lowerKey);
            if (it == m_headers.end())
            {
                std::stringstream ss;
                ss << defaultValue;
                return ss.str();
            }

            return it->second;
        }

        template<typename T>
        inline std::string Headers::find(const std::string & key, const T & defaultValue) const
        {
            std::string lowerKey;
            lowerKey.resize(key.size(), 0);
            std::transform(key.begin(), key.end(), lowerKey.begin(), [](int c) -> char {return static_cast<char>(::tolower(c)); });

            auto it = m_headers.find(lowerKey);
            if (it == m_headers.end())
            {
                return std::to_string(defaultValue);
            }

            return it->second;
        }

        template<>
        inline std::string Headers::find(const std::string & key, const char * defaultValue) const
        {
            std::string lowerKey;
            lowerKey.resize(key.size(), 0);
            std::transform(key.begin(), key.end(), lowerKey.begin(), [](int c) -> char {return static_cast<char>(::tolower(c)); });

            auto it = m_headers.find(lowerKey);
            if (it == m_headers.end())
            {
                return defaultValue;
            }

            return it->second;
        }

    }

}