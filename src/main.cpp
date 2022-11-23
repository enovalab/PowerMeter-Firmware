#ifndef PIO_UNIT_TESTING

#include "Measuring/PowerMeter.h"
#include "Connectivity/RestAPI.h"
#include "defines.h"
#include <AsyncWebSocket.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <StreamAverage.h>
#include <ArduinoLog.h>

Measuring::PowerMeter powerMeter(PIN_U, PIN_I);
AsyncWebServer server(80);
Connectivity::RestAPI api(80, "/api");

void makeAP()
{
    Serial.println("AP Mode");
    File metaFile = SPIFFS.open("/data/meta.json", FILE_WRITE, true);
    StaticJsonDocument<300> metaJson;
    deserializeJson(metaJson, metaFile);
    WiFi.mode(WIFI_AP);
    WiFi.softAP("RESTAPI AP", "HelloWorld!");
    Serial.println(WiFi.softAPIP());
    metaJson["ip"] = WiFi.softAPIP().toString();
    serializeJson(metaJson, metaFile);    
}

void makeSTA()
{
    Serial.println("STA Mode");
    File credentialsFile = SPIFFS.open("/data/credentials.json", FILE_READ);
    WiFi.mode(WIFI_STA);
    StaticJsonDocument<100> credentialsJson;
    deserializeJson(credentialsJson, credentialsFile);
    const char* ssid = credentialsJson["ssid"];
    const char* password = credentialsJson["password"];
    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED);
    Serial.println(WiFi.localIP());
}

void setup()
{
    Serial.begin(115200);
    SPIFFS.begin();
    
    
    pinMode(PIN_RELAY, OUTPUT);
    digitalWrite(PIN_RELAY, HIGH);

    powerMeter.calibrate(CAL_U, CAL_I, CAL_PHASE);

    makeAP();
    
    DefaultHeaders::Instance().addHeader("Access-Control-Request-Method", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Expose-Headers", "*");  
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

    server.on("/meta", HTTP_GET, [](AsyncWebServerRequest* request){
        request->send(SPIFFS, "/data/meta.json", "application/json");
    });

    server.on("/momentary", HTTP_GET, [](AsyncWebServerRequest* request){
        StaticJsonDocument<200> momentaryJson;
        Measuring::ACPower power = powerMeter.measure();
        momentaryJson["voltage"] = power.getVoltageRms();
        momentaryJson["current"] = power.getCurrentRms();
        momentaryJson["active"] = power.getActivePower();
        momentaryJson["apparent"] = power.getApparentPower();
        momentaryJson["reactive"] = power.getReactivePower();
        momentaryJson["factor"] = power.getPowerFactor();
        AsyncResponseStream* response = request->beginResponseStream("application/json");
        serializeJson(momentaryJson, *response);
        request->send(response);
    });



    server.on("/history/day", HTTP_GET, [](AsyncWebServerRequest* request){
        request->send(SPIFFS, "/data/history/day.json", "application/json");
    });

    server.on("/history/week", HTTP_GET, [](AsyncWebServerRequest* request){
        request->send(SPIFFS, "/data/history/week.json", "application/json");
    });

    server.on("/history/month", HTTP_GET, [](AsyncWebServerRequest* request){
        request->send(SPIFFS, "/data/history/month.json", "application/json");
    });
    
    server.on("/history/year", HTTP_GET, [](AsyncWebServerRequest* request){
        request->send(SPIFFS, "/data/history/year.json", "application/json");
    });

    server.on("/history/decade", HTTP_GET, [](AsyncWebServerRequest* request){
        request->send(SPIFFS, "/data/history/decade.json", "application/json");
    });


    server.on("/log", HTTP_GET, [](AsyncWebServerRequest* request){
        request->send(SPIFFS, "/log/log.log", "text/plain");
    });

    server.on("/log", HTTP_DELETE, [](AsyncWebServerRequest* request){
        StaticJsonDocument<100> responseJson;
        responseJson["success"] = SPIFFS.remove("/log/log.log");
        AsyncResponseStream* response = request->beginResponseStream("application/json");
        serializeJson(responseJson, *response);
        request->send(response);
    });

    server.serveStatic("/", SPIFFS, "/device_app").setDefaultFile("index.html");

    server.begin();
}

void loop()
{
    
}

#endif