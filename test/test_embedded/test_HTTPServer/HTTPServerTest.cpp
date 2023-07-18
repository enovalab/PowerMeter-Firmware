#include "Connectivity/HTTPServer.h"
#include "Diagnostics/Log.h"
#include <Arduino.h>
#include <WiFi.h>

using namespace Connectivity;

HTTPServer server(80, {
    {"Access-Control-Request-Method", "*"},
    {"Access-Control-Expose-Headers", "*"},
    {"Access-Control-Allow-Methods", "*"},
    {"Access-Control-Allow-Origin", "*"},
    {"Access-Control-Allow-Headers", "*"}
});

void setup()
{
    Serial.begin(115200);
    WiFi.begin("WLAN-T-Com", "GxvuzR8znV");
    delay(3000);
    Serial.println(WiFi.localIP());

    server.start();

    server.registerURI("/", HTTP::Method::Get, [](HTTPServer::Request request){
        Diagnostics::Logger[Level::Debug] << request.uri << '\n' << request.body << std::endl;
        return HTTPServer::Response("<h1>Hello World!</h1>", "text/html");
    });

    server.registerURI("/", HTTP::Method::Post, [](HTTPServer::Request request){
        Diagnostics::Logger[Level::Debug] << request.body << '\n' << request.body << std::endl;
        return HTTPServer::Response(request.body, "text/plain");
    });
    

}

void loop()
{
}