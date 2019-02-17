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

#ifndef WEPP_URI_HPP
#define WEPP_URI_HPP

#include "wepp/build.hpp"
#include <string>

/**
* Wepp namespace.
*
*/
namespace Wepp
{

    /**
    * Uniform Resource Identifier structure and parser.
    *
    * @see https://en.wikipedia.org/wiki/Uniform_Resource_Identifier
    *
    */
    class WEPP_API Uri
    {

    public:

        /**
        * Constuctor.
        * Initializing empty URI data structure.
        *
        */
        Uri();

        /**
        * Constuctor.
        * Initializing by parsing input data.
        *
        * @param[in] uri - Input uri to parse.
        * @see parse
        *
        */
        Uri(const char * uri);

        /**
        * Constuctor.
        * Initializing by parsing input data.
        *
        * @param[in] uri - Input uri to parse.
        * @see parse
        *
        */
        Uri(const std::string & uri);

        /**
        * Parsing input data and populating URI data structure.
        *
        * @param[in] uri - data to parse.
        * @see parse
        *
        */
        bool parse(const char * uri);

        /**
        * Parsing input data and populating URI data structure.
        *
        * @param[in] uri - data to parse.
        * @see parse
        *
        */
        bool parse(const std::string & uri);

        /**
        * Wiping the entire URI data structure.
        *
        */
        void clear();

        /**
        * Checks if the URI data structure is empty.
        *
        */
        bool isEmpty();

        std::string schema;    /**< Schema name. */
        std::string authority; /**< Authority string. */
        std::string path;      /**< Resource path. */
        std::string query;     /**< Query string. */
        std::string fragment;  /**< Fragment flag. */

    };

}

#endif