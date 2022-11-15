#ifndef JSONAPI_H
#define JSONAPI_H

#include <ESPAsyncWebServer.h>
#include <json.hpp>
#include <functional>
#include <string>
#include <initializer_list>

namespace Connectivity
{   
    AsyncWebServer Server(80);

    namespace RestApi
    {
        using JsonCallback_t = std::function<void(json)>;

        void createEndpoint(
            std::string uri,
            std::initializer_list<WebRequestMethod> methods, 
            std::string dataFilePath,
            JsonCallback_t callback = [](json data){},
            bool allowCors = true
        );

        void createEndpoint(
            std::string uri, 
            std::initializer_list<WebRequestMethod> methods, 
            json data,
            JsonCallback_t callback = [](json data){},
            bool allowCors = true
        );
    }
}

#endif