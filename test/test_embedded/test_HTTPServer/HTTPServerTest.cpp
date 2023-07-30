#include "HTTPSServer/HTTPSServer.h"
#include "Encryption/Certificate.h"
#include "Encryption/PrivateKey.h"
#include "Log/Log.h"
#include <Arduino.h>
#include <WiFi.h>

HTTPSServer server(
    Encryption::Certificate,
    sizeof(Encryption::Certificate),
    Encryption::PrivateKey,
    sizeof(Encryption::PrivateKey),
    443,
    {
        {"Access-Control-Request-Method", "*"},
        {"Access-Control-Expose-Headers", "*"},
        {"Access-Control-Allow-Methods", "*"},
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Headers", "*"}
    }
);

void setup()
{
    Serial.begin(115200);
    WiFi.begin("WLAN-T-Com", "GxvuzR8znV");
    delay(3000);
    Serial.println(WiFi.localIP());

    server.start();

    server.registerURI("/", HTTP::Method::GET, [](HTTPServer::Request){
        return HTTPServer::Response("<h1>Hello World!</h1>", "text/html");
    });

    server.registerURI("/", HTTP::Method::POST, [](const HTTPServer::Request& request){
        return HTTPServer::Response(request.body, "text/plain");
    });
}

void loop()
{
}