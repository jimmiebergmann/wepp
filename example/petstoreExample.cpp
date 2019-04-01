#include "wepp/http/server.hpp"
#include <mutex>
#include <map>
#include <iostream>

using namespace Wepp;

int main()
{
    Http::Server server;
    std::mutex mutex;
    std::map<std::string, std::string> pets;

    // GET /pets - Gets list of all available pets.
    server.route["GET"]["/pets"] = [&mutex, &pets](const Http::Request &, Http::Response & response)
    {
        std::lock_guard<std::mutex> lock(mutex);

        for (auto it = pets.begin(); it != pets.end(); it++)
        {
            response << it->first << ": " << it->second << "\r\n";
        }
    };

    // POST /pets - Add or update pet in store.
    server.route["POST"]["/pets"] = [&mutex, &pets](const Http::Request & request, Http::Response & response)
    {
        auto & headers = request.headers();

        auto nameIt = headers.find("name");
        auto typeIt = headers.find("type");
        if (nameIt == headers.end() || typeIt == headers.end())
        {
            response << "Missing headers.\r\n";
            response.status(Http::Status::BadRequest);
            return;
        }

        std::lock_guard<std::mutex> lock(mutex);

        auto petIt = pets.find(nameIt->second);
        if (petIt != pets.end())
        {
            petIt->second = typeIt->second;
            response << "Updated pet: " << nameIt->second << ": " << typeIt->second << "\r\n";
        }
        else
        {
            pets.insert({ nameIt->second, typeIt->second });
            response << "Added new pet: " << nameIt->second << ": " << typeIt->second << "\r\n";
        }
        
    };

    // POST /stop - Remotely stopping server.
    server.route["GET"]["/stop"] = [&mutex, &server](const Http::Request &, Http::Response & response)
    {
        std::lock_guard<std::mutex> lock(mutex);

        server.stop();
        response << "Stopping server.\r\n";
    };

    server.start(80);
    server.stopped().wait();
    std::cout << "Stopped." << std::endl;
    return 0;
}