#ifdef ESP32

#include "Connectivity/RestAPI.h"
#include "Diagnostics/Log.h"
#include "Diagnostics/ExceptionTrace.h"


using namespace Connectivity;


RestAPI::RestAPI(HTTPServer& server, const std::string& baseURI) noexcept : 
    m_server(server),
    m_baseURI(baseURI)
{
    m_server.handle(baseURI, HTTP::Method::Options, [](HTTPServer::Request){
        return HTTPServer::Response("", "", HTTP::StatusCode::NoContent, {
            {"Access-Control-Request-Method", "*"},
            {"Access-Control-Expose-Headers", "*"},
            {"Access-Control-Allow-Methods", "*"},
            {"Access-Control-Allow-Origin", "*"},
            {"Access-Control-Allow-Headers", "*"}
        });
    });
}


void RestAPI::handle(HTTP::Method method, const std::string& uri, const JsonHandler& handlerCallback) noexcept
{
    m_server.handle(m_baseURI + uri, method, [handlerCallback](const HTTPServer::Request& request){
        JsonResponse response("", HTTP::StatusCode::InternalServerError);
        try
        {
            json requestData = json::parse(request.body);
            response = handlerCallback(requestData);
        }
        catch(...)
        {
            response.data = Diagnostics::ExceptionTrace::what();

        }
        return HTTPServer::Response(response.data, "application/json", response.status);
    });
}

#endif