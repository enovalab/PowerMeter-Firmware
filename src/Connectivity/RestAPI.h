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
        using HeaderList = std::vector<Connectivity::HTTP::Header>;

        RestAPI(
            AsyncWebServer* server, 
            const std::string& baseURI = "/", 
            const HeaderList& defaultHeaders = {}
        ) noexcept;

        void handle(
            HTTP::Method method, 
            const std::string& endpointURI, 
            const JsonHandler& handler,
            const HeaderList& headers = {}
        ) noexcept;

    private:
        void handleWithoutBody(
            HTTP::Method method, 
            const std::string& endpointURI, 
            const JsonHandler& handler, 
            const HeaderList& headers
        ) noexcept;

        void handleWithBody(
            HTTP::Method method, 
            const std::string& endpointURI, 
            const JsonHandler& handler, 
            const HeaderList& headers
        ) noexcept;

        void handleHead(const std::string& endpointURI, const HeaderList& headers) noexcept;
        void addHeaders(AsyncWebServerResponse* response, const HeaderList& headers) noexcept;

        AsyncWebServer* m_server;
        std::string m_baseURI;
        HeaderList m_defaultHeaders;
    };
}

#endif
