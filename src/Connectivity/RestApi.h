#ifndef CONNECTIVITY_RESTAPI_H
#define CONNECTIVITY_RESTAPI_H

#include <ESPAsyncWebServer.h>
#include <json.hpp>
#include <functional>
#include <string>

namespace Connectivity
{
    class RestAPI
    {
    public:
        struct JsonResponse
        {
            json data;
            uint16_t statusCode = 200;
            std::string errorMessage = "";
        };

        using JsonHandler = std::function<JsonResponse(const json&)>;

        RestAPI(AsyncWebServer* server, const std::string& baseURI = "/", bool allowCORS = true);
        void handle(WebRequestMethod method, const std::string& endpointURI, const JsonHandler& handler);

    private:
        void addCORSHeaders(AsyncWebServerResponse* response);

        AsyncWebServer* m_server;
        std::string m_baseURI;
        bool m_allowCORS;
    };
}

#endif