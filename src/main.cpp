#include "Measuring/PowerMeter.h"
#include "Logging/Logger.h"
#include <ESPAsyncWebServer.h>
#include <iostream>
#include <string>
#include <SPIFFS.h>

AsyncWebServer server(80);
Logging::Log Logger(Logging::Level::Verbose, std::cout, true);


void setup()
{
    Serial.begin(115200);
    SPIFFS.begin();

    WiFi.mode(WIFI_AP);
    WiFi.softAP("Power Meter AP", "Katek1976");   

    server.on("/log", HTTP_GET, [](AsyncWebServerRequest* request){
        const char* a = SOURCE_LOCATION;
        Logger[Logging::Level::Verbose] << SOURCE_LOCATION << "This is Verbose\n";
        request->send(SPIFFS, "/log/log.log", "text/plain");
    });
    server.begin();
    
}

void loop()
{
}