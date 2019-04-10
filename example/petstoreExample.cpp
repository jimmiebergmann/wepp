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

    // On any error.
    server.onAnyError = [](Http::Response & response)
    {
        response << Http::getStatusAsString(response.status()) << " (" << std::to_string(static_cast<uint32_t>(response.status())) << ").";
    };

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

        auto name = headers.find("name");
        auto type = headers.find("type");

        if (!name.size() || !type.size())
        {
            response << "Missing headers.\r\n";
            response.status(Http::Status::BadRequest);
            return;
        }

        std::lock_guard<std::mutex> lock(mutex);

        auto pet = pets.find(name);
        if (pet != pets.end())
        {
            pet->second = type;
            response << "Updated pet: " << name << ": " << type << "\r\n";
        }
        else
        {
            pets.insert({ name, type });
            response << "Added new pet: " << name << ": " << type << "\r\n";
        }
        
    };


    // POST /stop - Remotely stopping server.
    server.route[""]["/stop"] = [&mutex, &server](const Http::Request &, Http::Response & response)
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