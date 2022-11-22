#ifndef RESTAPI_H
#define RESTAPI_H

#include <ESPAsyncWebServer.h>
#include <json.hpp>
#include <functional>
#include <string>

namespace Connectivity
{
    using JsonCallback = std::function<void(json)>;

    class RestAPI
    {
    public:
        RestAPI(AsyncWebServer server, std::string baseUri = "/", bool allowCors = true);
        createEndpointRead(std::string endpointUri, std::string filePath, JsonCallback callback = [](json){});
        createEndpointWrite(std::string endpointUri, std::string filePath);
        

    private:
        AsyncWebServer m_server;
        std::string m_baseUri;
        bool m_allowCors;
    };
}

#endif