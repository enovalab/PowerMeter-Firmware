#ifdef ESP32

#include "RestAPI.h"
#include "ExceptionTrace/ExceptionTrace.h"


RestAPI::RestAPI(HTTPServer& server, const std::string& baseURI) noexcept : 
    m_server(server),
    m_baseURI(baseURI)
{}


void RestAPI::registerURI(const std::string& uri, HTTP::Method method, const JsonHandler& handlerCallback) noexcept
{
    m_server.registerURI(m_baseURI + uri, method, [handlerCallback](const HTTPServer::Request& request){
        JsonResponse response(json(), HTTP::StatusCode::InternalServerError);
        try
        {
            json requestData;
            if(!request.body.empty())
                requestData = json::parse(request.body);
            response = handlerCallback(requestData);
        }
        catch(...)
        {
            response.data = ExceptionTrace::what();
        }
        return HTTPServer::Response(response.data.dump(1, '\t'), "application/json", response.status);
    });

    m_server.registerURI(m_baseURI + uri, HTTP::Method::OPTIONS, [](const HTTPServer::Request& request){
        return HTTPServer::Response("", "", HTTP::StatusCode::NoContent, {
            {"Access-Control-Request-Method", "*"},
            {"Access-Control-Allow-Headers", "*"},
            {"Access-Control-Allow-Methods", "*"}
        });
    });
}

#endif