#ifndef CONNECTIVITY_RESTAPI_H
#define CONNECTIVITY_RESTAPI_H

#include <ESPAsyncWebServer.h>
#include <json.hpp>
#include <functional>
#include <string>

namespace Connectivity
{
    using JsonCallback = std::function<void(json)>;
    using JsonGenerator = std::function<json()>;

    class RestAPI
    {
    public:
        RestAPI(AsyncWebServer server, std::string baseUri = "/", bool allowCors = true);
        // createEndpointRead(std::string endpointUri, std::string filePath);
        // createEndpointRead(std::string endpointUri, JsonGenerator generator);
        // createEndpointWrite(std::string endpointUri, std::string filePath, JsonCallback callback = [](json){});
        // createEndpointWrite(std::string endpointUri, JsonCallback callback = [](json){});

        

    private:
        AsyncWebServer m_server;
        std::string m_baseUri;
        bool m_allowCors;
    };
}

#endif