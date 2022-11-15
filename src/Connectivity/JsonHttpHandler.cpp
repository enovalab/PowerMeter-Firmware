#include "JsonHttpHandler.h"
#include "Logging/Logger.h"

using namespace Connectivity;

template<WebRequestMethod method>
constexpr JsonHttpHandler JsonHttpHandler::makeDefaultHandler()
{
    JsonHttpHandler defaultHandler;
    switch (method)
    {
    case HTTP_PATCH:
        defaultHandler.requestHandler = [](AsyncWebServerRequest* request){};
        defaultHandler.uploadHandler = [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){};
        defaultHandler.bodyHandler = [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {

        };        
        break;



    case HTTP_POST:
        defaultHandler.requestHandler = [](AsyncWebServerRequest* request){};
        defaultHandler.uploadHandler = [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){};
        defaultHandler.bodyHandler = [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
            
        };
        break;



    case HTTP_PUT:
        defaultHandler.requestHandler = [](AsyncWebServerRequest* request){

        };

        defaultHandler.uploadHandler = [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
            
        };

        defaultHandler.bodyHandler = [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {

        };
        break;



    case HTTP_DELETE:
        defaultHandler.requestHandler = [](AsyncWebServerRequest* request){

        };

        defaultHandler.uploadHandler = [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
            
        };

        defaultHandler.bodyHandler = [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {

        };
        break;



    case HTTP_GET:
        defaultHandler.requestHandler = [](AsyncWebServerRequest* request){

        };

        defaultHandler.uploadHandler = [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
            
        };

        defaultHandler.bodyHandler = [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {

        };
        break;



    case HTTP_OPTIONS:
        defaultHandler.requestHandler = [](AsyncWebServerRequest* request){

        };

        defaultHandler.uploadHandler = [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
            
        };

        defaultHandler.bodyHandler = [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {

        };
        break;
    
    
    default:
        defaultHandler.requestHandler = [](AsyncWebServerRequest* request){
            Logger::Log.warningln(F("Unhandled HTTP-Request"));
        };
        defaultHandler.uploadHandler = [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){}
        defaultHandler.bodyHandler = [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {};
        break;
    }
}

