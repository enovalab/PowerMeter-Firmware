#ifndef CONNECTIVITY_RESTAPI_H
#define CONNECTIVITY_RESTAPI_H

#include "HTTPServer.h"

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
            HTTPServer& server, 
            const std::string& baseURI = "/"
        ) noexcept;

        void handle(
            HTTP::Method method, 
            const std::string& uri, 
            const JsonHandler& handlerCallback
        ) noexcept;

    private:
        HTTPServer& m_server;
        std::string m_baseURI;
    };
}

#endif
