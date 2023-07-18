#ifdef ESP32

#include "Connectivity/RestAPI.h"
#include "Diagnostics/Log.h"
#include "Diagnostics/ExceptionTrace.h"


using namespace Connectivity;


RestAPI::RestAPI(HTTPServer& server, const std::string& baseURI) noexcept : 
    m_server(server),
    m_baseURI(baseURI)
{
}


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
            response.data = Diagnostics::ExceptionTrace::what();
        }
        return HTTPServer::Response(response.data.dump(1, '\t'), "application/json", response.status);
    });

    m_server.registerURI(m_baseURI + uri, HTTP::Method::Options, [](HTTPServer::Request){
        return HTTPServer::Response("", "", HTTP::StatusCode::NoContent, {
            {"Access-Control-Request-Method", "*"},
            {"Access-Control-Expose-Headers", "*"},
            {"Access-Control-Allow-Methods", "*"},
            {"Access-Control-Allow-Origin", "*"},
            {"Access-Control-Allow-Headers", "*"}
        });
    });
}

#endif