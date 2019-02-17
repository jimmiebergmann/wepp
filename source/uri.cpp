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

#include "uri.hpp"
#include <regex>

namespace Wepp
{

    Uri::Uri()
    { }

    Uri::Uri(const char * uri)
    {
        parse(uri);
    }

    Uri::Uri(const std::string & uri)
    {
        parse(uri);
    }

    bool Uri::parse(const char * uri)
    {
        std::regex fullRegex(R"(^(?:([a-zA-Z0-9$\-_.+!*‘(),%]*)?:)?(?://([a-zA-Z0-9$\-_.+!*‘(),%:@]*))?([a-zA-Z0-9$\-_.+!*‘(),/%:=@]*)?(?:\?([a-zA-Z0-9$\-_.+!*‘(),%=&]*))?(?:#([a-zA-Z0-9$\-_.+!*‘(),%]*))?)");
        std::cmatch matches;
        
        if (!std::regex_search(uri, matches, fullRegex))
        {
            return false;
        }

        schema = matches[1];
        authority = matches[2];
        path = matches[3];
        query = matches[4];
        fragment = matches[5];

        return true;
    }

    bool Uri::parse(const std::string & uri)
    {
        return parse(uri.c_str());
    }

    void Uri::clear()
    {
        schema = "";
        authority = "";
        path = "";
        query = "";
        fragment = "";
    }

    bool Uri::isEmpty()
    {
        return schema.empty() && authority.empty() && path.empty() && query.empty() && fragment.empty();
    }

}
