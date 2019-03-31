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

#ifndef WEPP_HTTP_BODY_HPP
#define WEPP_HTTP_BODY_HPP

#include "wepp/build.hpp"
#include <string>
#include <vector>

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
        * Http body class.
        *
        * The data is internally stored in a std::vector.
        *
        */
        class WEPP_API Body
        {

        public:

            /**
            * Default constructor.
            *
            */
            Body();

            /**
            * Gets data of body.
            *
            */
            char * data();

            /**
            * Gets const data of body.
            *
            */
            const char * data() const;

            /**
            * Gets size of body.
            *
            */
            size_t size() const;

            /**
            * Append data to body
            *
            */
            Body & append(const char * data, const size_t size);

            /**
            * Clear body data.
            *
            */
            Body & clear();

            /**
            * Assigning operator.
            *
            */
            Body & operator =(const std::string & string);

            /**
            * Input steaming operator.
            *
            */
            Body & operator <<(const std::string & string);

        private:

            std::vector<char> m_data;     /**< Unique pointer for data.*/

        };

    }

}

#endif
