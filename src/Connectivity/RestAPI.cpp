#include "Connectivity/RestAPI.h"
#include "Logging/Log.h"
#include "ErrorManagement/ExceptionStack.h"

using namespace Connectivity;


RestAPI::RestAPI(AsyncWebServer* server, const std::string& baseURI, bool allowCORS) : 
    m_server(server),
    m_baseURI(baseURI),
    m_allowCORS(allowCORS)
{
    if(m_allowCORS)
    {
        m_server->on(m_baseURI.c_str(), HTTP_OPTIONS, [](AsyncWebServerRequest* request) {
            request->send(204);
        });
    }
}


void RestAPI::handle(WebRequestMethod method, const std::string& endpointURI, const JsonHandler& handler)
{
    switch(method)
    {
    case HTTP_GET:
        handleGet(endpointURI, handler);
        handleHead(endpointURI, handler);
        break;
    
    case HTTP_DELETE:
        handleDelete(endpointURI, handler);
        break;

    default:
        handleDefault(method, endpointURI, handler);
        break;
    }
}


void RestAPI::handleGet(const std::string& endpointURI, const JsonHandler& handler)
{
    m_server->on((m_baseURI + endpointURI).c_str(), HTTP_GET, [handler, this](AsyncWebServerRequest* request){
        JsonResponse jsonResponse;
        try
        {
            jsonResponse = handler(json());
        }
        catch(const std::exception& e)
        {
            jsonResponse.statusCode = 500;
        }
        catch(...)
        {
            jsonResponse.statusCode = 500;
            jsonResponse.errorMessage = "Unexpected Exception";
            Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << "Unexpected Exception" << std::endl;
        }

        AsyncWebServerResponse* response;

        if(jsonResponse.statusCode >= 200 && jsonResponse.statusCode < 300)
            response = request->beginResponse(jsonResponse.statusCode, "application/json", jsonResponse.data.dump().c_str());
        else
            response = request->beginResponse(jsonResponse.statusCode, "text/plain", jsonResponse.errorMessage.c_str());

        addCORSHeaders(response);
        request->send(response);

        request->send(200);
    });
}


void RestAPI::handleHead(const std::string& endpointURI, const JsonHandler& handler)
{
    m_server->on((m_baseURI + endpointURI).c_str(), HTTP_HEAD, [handler, this](AsyncWebServerRequest* request){
        JsonResponse jsonResponse;
        try
        {
            jsonResponse = handler(json());
        }
        catch(const std::exception& e)
        {
            jsonResponse.statusCode = 500;
            jsonResponse.errorMessage = e.what();
            Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << e.what() << std::endl;
        }
        catch(...)
        {
            jsonResponse.statusCode = 500;
            jsonResponse.errorMessage = "Unexpected Exception";
            Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << "Unexpected Exception" << std::endl;
        }

        AsyncWebServerResponse* response;

        if(jsonResponse.statusCode >= 200 && jsonResponse.statusCode < 300)
            response = request->beginResponse(jsonResponse.statusCode, "application/json");
        else
            response = request->beginResponse(jsonResponse.statusCode, "text/plain", jsonResponse.errorMessage.c_str());

        addCORSHeaders(response);
        request->send(response);
    });
}


void RestAPI::handleDelete(const std::string& endpointURI, const JsonHandler& handler)
{
    m_server->on((m_baseURI + endpointURI).c_str(), HTTP_DELETE, [handler, this](AsyncWebServerRequest* request){
        JsonResponse jsonResponse;
        try
        {
            jsonResponse = handler(json());
        }
        catch(const std::exception& e)
        {
            jsonResponse.statusCode = 500;
            jsonResponse.errorMessage = e.what();
            Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << e.what() << std::endl;
        }
        catch(...)
        {
            jsonResponse.statusCode = 500;
            jsonResponse.errorMessage = "Unexpected Exception";
            Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << "Unexpected Exception" << std::endl;
        }

        AsyncWebServerResponse* response;

        if(jsonResponse.statusCode >= 200 && jsonResponse.statusCode < 300)
            response = request->beginResponse(jsonResponse.statusCode, "application/json");
        else
            response = request->beginResponse(jsonResponse.statusCode, "text/plain", jsonResponse.errorMessage.c_str());

        addCORSHeaders(response);
        request->send(response);
    });
}


void RestAPI::handleDefault(WebRequestMethod method, const std::string& endpointURI, const JsonHandler& handler)
{
    m_server->on((m_baseURI + endpointURI).c_str(), method, 
        [](AsyncWebServerRequest*){},
        [](AsyncWebServerRequest*, const String&, size_t, uint8_t*, size_t, bool){},
        [handler, this](AsyncWebServerRequest* request, uint8_t* rawData, size_t length, size_t index, size_t total) {
            JsonResponse jsonResponse;
            try
            {
                jsonResponse = handler(json::parse(reinterpret_cast<char*>(rawData)));;
            }
            catch(const std::exception& e)
            {
                jsonResponse.statusCode = 500;
                jsonResponse.errorMessage = e.what();
                Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << e.what() << std::endl;
            }
            catch(...)
            {
                jsonResponse.statusCode = 500;
                jsonResponse.errorMessage = "Unexpected Exception";
                Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << "Unexpected Exception" << std::endl;
            }
            AsyncWebServerResponse* response;

            if(jsonResponse.statusCode >= 200 && jsonResponse.statusCode < 300)
                response = request->beginResponse(jsonResponse.statusCode, "application/json", jsonResponse.data.dump().c_str());
            else
                response = request->beginResponse(jsonResponse.statusCode, "text/plain", jsonResponse.errorMessage.c_str());

            addCORSHeaders(response);
            request->send(response);
        }
    );
}


void RestAPI::addCORSHeaders(AsyncWebServerResponse* response)
{
    if(m_allowCORS)
    {
        response->addHeader("Access-Control-Request-Method", "*");
        response->addHeader("Access-Control-Expose-Headers", "*"); 
        response->addHeader("Access-Control-Allow-Methods", "*");
        response->addHeader("Access-Control-Allow-Origin", "*");
        response->addHeader("Access-Control-Allow-Headers", "*");
    }
}
