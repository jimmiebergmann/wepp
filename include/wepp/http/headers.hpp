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

#ifndef WEPP_HTTP_HEADERS_HPP
#define WEPP_HTTP_HEADERS_HPP

#include "wepp/build.hpp"
#include <string>
#include <sstream>
#include <map>
#include <algorithm>

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
        * Http headers class, containing map of header class.
        *
        * The data is internally stored in a std::map.
        *
        */
        class WEPP_API Headers
        {

        public:

            /**
            * Default constructor.
            *
            */
            Headers();

            /**
            * Gets number of headers.
            *
            */
            size_t size() const;

            /**
            * Gets begin iterator.
            *
            */
            std::map<std::string, std::string>::const_iterator begin() const;
            
            /**
            * Gets end iterator.
            *
            */
            std::map<std::string, std::string>::const_iterator end() const;

            /**
            * Checks if header exists with passed key value.
            *
            */
            bool exists(const std::string & key) const;

            /**
            * Set header value of key, by passing a reference value.
            *
            * @param value - std::string and any type allowed as parameter for std::to_string will work.
            *
            */
            template<typename T>
            Headers & set(const std::string & key, const T & value);

            /**
            * Set header value of key, by passing a pointer value.
            *
            * @param value - only const char * ( C string ) is allowed as input parameter.
            *
            */
            template<typename T>
            Headers & set(const std::string & key, const T * value);  

            /**
            * Unset and remove header from this container.
            *
            */
            Headers & unset(const std::string & key);

            /**
            * Find header value.
            *
            * @return std::string representing value.
            * Returns a zero length std::string if not found, or the value actually is empty.
            *
            */
            std::string find(const std::string & key) const;

            /**
            * Find header by passed reference value, returning defualt value if not found.
            *
            * @return std::string representing value.
            *         Passed default value is set to returned header if the key is not found.
            *
            */
            template<typename T>
            std::string find(const std::string & key, const T & defaultValue) const;


            /**
            * Find header by passed pointer value, returning defualt value if not found.
            *
            * @return std::string representing value.
            *         Passed default value is set to returned header if the key is not found.
            *
            */
            template<typename T>
            std::string find(const std::string & key, const T * defaultValue) const;

            /**
            * Clear all headers.
            *
            */
            Headers & clear();

        private:

            std::map<std::string, std::string> m_headers; /**< Map of headers. */

        };

    }

}

#include "wepp/http/headers.inl"

#endif
