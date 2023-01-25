#ifndef CONNECTIVITY_RESTAPI_H
#define CONNECTIVITY_RESTAPI_H

#include "Connectivity/HTTP.h"

#include <json.hpp>
#include <functional>
#include <string>
#include <utility>

namespace Connectivity
{
    class RestAPI
    {
    public:
        struct JsonResponse
        {
            JsonResponse(const json& data = nullptr, HTTP::StatusCode status = HTTP::StatusCode::OK) :
                data(data),
                status(status)
            {}
            json data;
            HTTP::StatusCode status;
        };

        using JsonHandler = std::function<JsonResponse(const json&)>;

        RestAPI(AsyncWebServer* server, const std::string& baseURI = "/", bool allowCORS = true);
        void handle(HTTP::Method method, const std::string& endpointURI, const JsonHandler& handler);

    private:
        void handleWithoutBody(HTTP::Method method, const std::string& endpointURI, const JsonHandler& handler);
        void handleWithBody(HTTP::Method method, const std::string& endpointURI, const JsonHandler& handler);
        void handleHead(const std::string& endpointURI);
        void addCORSHeaders(AsyncWebServerResponse* response);

        AsyncWebServer* m_server;
        std::string m_baseURI;
        bool m_allowCORS;
    };
}

#endif
