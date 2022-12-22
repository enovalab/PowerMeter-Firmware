#ifndef CONNECTIVITY_RESTAPI_H
#define CONNECTIVITY_RESTAPI_H

#include <ESPAsyncWebServer.h>
#include <json.hpp>
#include <functional>
#include <string>
#include <utility>

namespace Connectivity
{
    class RestAPI
    {
    public:
        using JsonHandler = std::function<std::pair<json, uint16_t>(const json&)>;

        RestAPI(AsyncWebServer* server, const std::string& baseURI = "/", bool allowCORS = true);
        void handle(WebRequestMethod method, const std::string& endpointURI, const JsonHandler& handler);

    private:
        void handleWithoutBody(WebRequestMethod method, const std::string& endpointURI, const JsonHandler& handler);
        void handleWithBody(WebRequestMethod method, const std::string& endpointURI, const JsonHandler& handler);
        void handleHead(const std::string& endpointURI);
        void addCORSHeaders(AsyncWebServerResponse* response);

        AsyncWebServer* m_server;
        std::string m_baseURI;
        bool m_allowCORS;
    };
}

#endif
