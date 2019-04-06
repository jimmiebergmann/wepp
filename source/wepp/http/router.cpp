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
#include <set>
#include <memory>

namespace Wepp
{

    namespace Http
    {

        static const Router::CallbackFunc s_defaultCallbackFunc = nullptr;

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
            return this->operator[](getMethodAsString(method).c_str());
        }

        RouteMethod & Router::operator[](const std::string & method)
        {
            std::string methodName = method;
            std::transform(methodName.begin(), methodName.end(), methodName.begin(), [](int c) -> char {return static_cast<char>(::toupper(c)); });

            auto it = m_methods.find(methodName);
            if (it != m_methods.end())
            {
                return *it->second;
            }

            RouteMethod * routeMethod = new RouteMethod(methodName);
            m_methods.insert({ methodName, routeMethod });
            return *routeMethod;
        }

        const Router::CallbackFunc & Router::find(const std::string & method, const std::string & path, std::vector<std::string> & matches) const
        {
            auto it = m_methods.find(method);
            if (it == m_methods.end())
            {
                return s_defaultCallbackFunc;
            }

            return it->second->find(path, matches);
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

        RouteCallback & RouteMethod::operator[](const std::string & path)
        {
            std::vector<std::string> dirs;
            std::regex findDirRegex(R"(/?((?:[^/]*<[^>]*>)*[^/]*)?)"); //R"((?:/?((?:[^/]*<[^>]*>)*)?([^/]*)))");
            std::regex findTagRegex(R"(<(\\?.)*?>)");// R"(<[^>]*>)");
            std::regex findTagDataRegex(R"(<((?:\\?.)*?)>)");

            const char * iter = path.c_str();
            for (std::cmatch m; *iter && std::regex_search(iter, m, findDirRegex); iter += m[0].length())
            {
                dirs.push_back(m[1].length() ? m[1] : m[2]);
            }

            RouteNode * currentNode = &m_rootNode;
            for (auto const & dir : dirs)
            {
                auto it = currentNode->regularTree.find(dir);
                if (it != currentNode->regularTree.end())
                {
                    // Found regular node, move to next dir.
                    currentNode = it->second;
                    continue;
                }

                it = currentNode->tagTree.find(dir);
                if (it != currentNode->tagTree.end())
                {
                    // Found tag key node, move to next dir.
                    currentNode = it->second;
                    continue;
                }

                // Insert new node, but first check if it's a regular or a tag node.
                if (std::regex_search(dir, findTagRegex))
                {
                    std::unique_ptr<RouteNode> newRouteNode(new RouteNode());

                    std::string tagRegexDir = dir;
                    std::string tagDir = dir;
                    size_t currSearchPos = 0;

                    // Find all tags
                    for (std::smatch match; std::regex_search(tagRegexDir, match, findTagDataRegex);tagRegexDir = match.suffix())
                    {
                        size_t matchLength = match.length();
                        size_t matchPosition = match.position();
                        std::string groupedMatch;

                        if (match[1].str().size())
                        {
                            groupedMatch = "(" + match[1].str() + ")";
                        }
                        else
                        {
                            groupedMatch = "(.*)";
                        }

                        // Replace tag with regex
                        tagDir.replace(matchPosition + currSearchPos, matchLength, groupedMatch);
                        currSearchPos += groupedMatch.length() + match.prefix().length();
                    }

                    // Add new route to previous node.
                    newRouteNode->dir = tagDir;
                    currentNode->tagTree.insert({ dir, newRouteNode.get() });


                    currentNode = newRouteNode.release();
                    continue;
                }

                // Insert regular node.
                RouteNode * newRouteNode = new RouteNode();
                newRouteNode->dir = dir;
                currentNode->regularTree.insert({ dir, newRouteNode});
                currentNode = newRouteNode;
            }

            if (!currentNode->routeCallback)
            {
                currentNode->routeCallback = new RouteCallback();
            }

            return *(currentNode->routeCallback);
        }

        const Router::CallbackFunc & RouteMethod::find(const std::string & path, std::vector<std::string> & matches) const
        {
            matches.clear();

            std::vector<std::string> dirs;
            std::regex findDirRegex(R"((?:/?([^/]*)))");

            const char * iter = path.c_str();
            for (std::cmatch m; *iter && std::regex_search(iter, m, findDirRegex); iter += m[0].length())
            {
                dirs.push_back(m[1]);
            }

            const RouteNode * currentNode = &m_rootNode;
            for (auto const & dir : dirs)
            {
                // Find regular dir and more to next one.
                auto it = currentNode->regularTree.find(dir);
                if (it != currentNode->regularTree.end())
                {
                    currentNode = it->second;
                    continue;
                }

                // Find tag node using regex.
                bool foundTagDir = false;
                for (it = currentNode->tagTree.begin(); it != currentNode->tagTree.end(); it++)
                {
                    std::regex findRegex(it->second->dir);
                    std::smatch match;

                    if (std::regex_search(dir, match, findRegex))
                    {
                        foundTagDir = true;

                        for (size_t i = 1; i < match.size(); i++)
                        {
                            matches.push_back(match[i]);
                        }

                        currentNode = it->second;
                        break;
                    }
                }

                if (foundTagDir)
                {
                    continue;
                }

                // Failed to find any route.
                return s_defaultCallbackFunc;
            }

            if (currentNode->routeCallback)
            {
                return currentNode->routeCallback->callback;
            }
            return s_defaultCallbackFunc;
        }

        // Route node.
        RouteMethod::RouteNode::RouteNode() :
            routeCallback(nullptr)
        { }

        RouteMethod::RouteNode::~RouteNode()
        {
            for (auto it = regularTree.begin(); it != regularTree.end(); it++)
            {
                delete it->second;
            }

            for (auto it = tagTree.begin(); it != tagTree.end(); it++)
            {
                delete it->second;
            }

            if (routeCallback != nullptr)
            {
                delete routeCallback;
            }
        }


        // Route path class.
        RouteCallback::RouteCallback()
        { }

        RouteCallback & RouteCallback::operator=(const Router::CallbackFunc & p_callback)
        {
            callback = p_callback;
            return *this;
        }

        // OnRequestRouter implementation.
        OnRequestRouter::OnRequestRouter()
        { }

        OnRequestRouter::OnRequestRouter(const CallbackFunction & p_callback) :
            callback(p_callback)
        { }

        OnRequestRouter & OnRequestRouter::operator=(const CallbackFunction & p_callback)
        {
            callback = p_callback;
            return *this;
        }

    }

}
