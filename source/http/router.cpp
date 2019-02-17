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

#include "wepp/http/router.hpp"
#include <algorithm>
#include <regex>

namespace Wepp
{

    namespace Http
    {

        // Route class.
        Router::Router()
        { }

        Router::~Router()
        {
            for (auto it = m_methods.begin(); it != m_methods.end(); it++)
            {
                delete it->second;
            }
        }

        size_t Router::methodCount() const
        {
            return m_methods.size();
        }

        RouteMethod & Router::operator[](const Method method)
        {
            return this->operator[](getMethodName(method).c_str());
        }

        RouteMethod & Router::operator[](const std::string & method)
        {
            std::string methodName = method;
            std::transform(methodName.begin(), methodName.end(), methodName.begin(), ::toupper);

            auto it = m_methods.find(methodName);
            if (it != m_methods.end())
            {
                return *it->second;
            }

            RouteMethod * routeMethod = new RouteMethod(methodName);
            auto result = m_methods.insert({ methodName, routeMethod });
            return *routeMethod;
        }


        // Route method class.
        RouteMethod::~RouteMethod()
        {
        }

        RouteMethod::RouteMethod(const std::string & name) :
            m_name(name)
        { }

        const std::string & RouteMethod::name() const
        {
            return m_name;
        }

        // Regular node.
        RouteMethod::RegularNode::RegularNode() :
            routePath(nullptr)
        { }

        RouteMethod::RegularNode::~RegularNode()
        {
            for (auto it = regularTree.begin(); it != regularTree.end(); it++)
            {
                delete it->second;
            }

            for (auto it = tagTree.begin(); it != tagTree.end(); it++)
            {
                delete *it;
            }

            if (routePath != nullptr)
            {
                delete routePath;
            }
        }

        // Tag node.
        RouteMethod::TagNode::TagNode() :
            routePath(nullptr)
        { }

        RouteMethod::TagNode::~TagNode()
        {
            for (auto it = regularTree.begin(); it != regularTree.end(); it++)
            {
                delete it->second;
            }

            for (auto it = tagTree.begin(); it != tagTree.end(); it++)
            {
                delete *it;
            }

            if (routePath != nullptr)
            {
                delete routePath;
            }

            for (auto it = tags.begin(); it != tags.end(); it++)
            {
                delete *it;
            }
        }

        // ..
        RoutePath & RouteMethod::operator[](const std::string & path)
        {
            std::vector<std::string> dirs;

            std::regex dirRegex(R"((?:/?((?:[^/]*<[^>]*>)*)?([^/]*)))");
            const char * iter = path.c_str();
            for (std::cmatch m; *iter && std::regex_search(iter, m, dirRegex); iter += m[0].length())
            {
                dirs.push_back(m[2].length() ? m[2] : m[1]);
            }
            
            /*RouteTree * curRouteTree = &m_routeTree;
            for (auto const & dir : dirs)
            {
                auto it = curRouteTree->find(dir);
                if (it != curRouteTree->end())
                {
                    curRouteTree = &(it->second->routeTree);
                }
            }*/
           



            /*if (std::regex_match(path.c_str(), dirRegex))
            {
                for (std::smatch m; std::regex_search(path.c_str(), m, dirRegex); s = m.suffix())
                {
                    int foo = 5;
                    //std::cout << m[1] << "\t\t" << m[2] << "\t\t" << m[3] << std::endl;
                }
            }*/

           /* auto count = matches.size();
            auto length = matches.length();
            
            for (const auto & match : matches)
            {
                auto size = matches.size();
                int foo = 5;
            }*/


            // TEMP
            RoutePath a(nullptr);
            return a;
        }


        // Route path class.
        RoutePath::RoutePath(const Router::CallbackFunc & callback) :
            callback(callback)
        { }

        RoutePath & RoutePath::operator=(const Router::CallbackFunc & p_callback)
        {
            callback = p_callback;
            return *this;
        }
       
    }

}