#include "RestApi.h"
#include "Logging/Logger.h"
#include <fstream>

namespace Connectivity
{
    namespace RestApi
    {
        namespace
        {
            void addCorsHeaders(AsyncWebServerResponse* response)
            {
                response->addHeader("Access-Control-Request-Method", "*");
                response->addHeader("Access-Control-Expose-Headers", "*");  
                response->addHeader("Access-Control-Allow-Methods", "*");
                response->addHeader("Access-Control-Allow-Origin", "*");
                response->addHeader("Access-Control-Allow-Headers", "*");
            }
        }

        void createEndpoint(
            std::string uri,
            std::initializer_list<WebRequestMethod> methods, 
            std::string dataFilePath,
            JsonCallback_t callback = [](json data){},
            bool allowCors = true
        )
        {
            std::fstream dataFile("/spiffs" + dataFilePath, std::ios_base::out);
            json data = json::parse(dataFile);

            for(const auto& method : methods)
            {
                ArRequestHandlerFunction requestHandler = [](AsyncWebServerRequest* request){};
                ArUploadHandlerFunction uploadHandler = [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){};
                ArBodyHandlerFunction bodyHandler = [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){};

                switch(method)
                {
                case HTTP_GET:
                    requestHandler = [=](AsyncWebServerRequest* request){
                        std::string subUri = std::string(request->url().c_str());
                        subUri.erase(0, uri.length());
                        AsyncWebServerResponse* response;
                        try
                        {
                            response = request->beginResponse(200, "application/json", data.at(json::json_pointer(subUri)).dump().c_str());
                        }
                        catch(const std::exception& e)
                        {
                            Logger::Log.errorln(e.what());
                            response = request->beginResponse(500);
                        }

                        if(allowCors)
                            addCorsHeaders(response);
                        
                        request->send(response);
                    };
                }

                Server.on(uri.c_str(), method, requestHandler, uploadHandler, bodyHandler);
            }
        }

        void createEndpoint(
            std::string uri, 
            std::initializer_list<WebRequestMethod> methods, 
            json data,
            JsonCallback_t callback = [](json data){},
            bool allowCors = true
        )
        {
            
        }

    }
}