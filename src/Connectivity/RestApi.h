#ifndef JSONAPI_H
#define JSONAPI_H

#include <ESPAsyncWebServer.h>
#include <json.hpp>
#include <functional>
#include <string>
#include <initializer_list>

namespace Connectivity
{   
    using json = nlohmann::json;

    namespace RestApi
    {
        void attachEndpoint(std::string uri, std::initializer_list<WebRequestMethod> methods, std::string dataFilePath);
        void attachEndpoint(std::string uri, std::initializer_list<WebRequestMethod> methods, json data);
    }
}

#endif