#ifndef PIO_UNIT_TESTING

#include "Measuring/PowerMeter.h"
// #include "Logging/Logger.h"
#include <iostream>
#include "defines.h"
#include <ESPAsyncWebServer.h>
#include <iostream>
#include <SPIFFS.h>

AsyncWebServer server(80);


void setup()
{
    Serial.begin(115200);

    WiFi.mode(WIFI_AP);
    WiFi.softAP("Power Meter AP", "Katek1976");   

    server.on("/log", HTTP_GET, [](AsyncWebServerRequest* request){
        request->send(SPIFFS, "/log/log.log", "text/plain");
    });
    server.begin();

    class NullBuffer : public std::streambuf
    {
    public:
        int overflow(int c) { return c; }
    };
    NullBuffer null_buffer;
    std::ostream null_stream(&null_buffer);
    null_stream << "Nothing will be printed";
    
    // Logging::DefaultLogger[Logging::Level::Error] << "hello" << std::endl;
}

void loop()
{
}

#endif