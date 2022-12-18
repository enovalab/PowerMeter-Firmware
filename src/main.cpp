#include <Arduino.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <fstream>
#include <json.hpp>
#include "Logging/Log.h"
#include "Data/JsonURI.h"

AsyncWebServer server(80);

void setup()
{
    Serial.begin(115200);
    if(!SPIFFS.begin(true))
    {
        Serial.println("spiffs failed");
    }
    
    WiFi.mode(WIFI_STA);
    WiFi.begin("WLAN-T-Com", "GxvuzR8znV");
    while(WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println(WiFi.localIP());

    DefaultHeaders::Instance().addHeader("Access-Control-Request-Method", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Expose-Headers", "*");  
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

    server.on("/test", HTTP_GET, [](AsyncWebServerRequest* request){
        request->send(SPIFFS, "/test.json", "application/json");
    });

    server.on("/test", HTTP_PUT, 
        [](AsyncWebServerRequest *request) {},
        [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){},
        [](AsyncWebServerRequest *request, uint8_t *data, size_t length, size_t index, size_t total) {
            
            Data::JsonURI("/spiffs/test.json").serialize(json::parse(reinterpret_cast<char*>(data)));
            request->send(200);
        }
    );

    server.on("/test", HTTP_PATCH, 
        [](AsyncWebServerRequest *request) {},
        [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){},
        [](AsyncWebServerRequest *request, uint8_t *rawData, size_t length, size_t index, size_t total) {
            Data::JsonURI targetURI("/spiffs/test.json");
            json existingData = targetURI.deserialize();
            json newData =  json::parse(reinterpret_cast<char*>(rawData));
            existingData.update(newData);
            targetURI.serialize(existingData);
            request->send(200);
        }
    );

    server.begin();
}

void loop()
{
}