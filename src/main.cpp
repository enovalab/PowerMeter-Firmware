#include <Arduino.h>
#include <DataManager.h>
#include <ESPAsyncWebServer.h>
#include <WiFiManager.h>

#define voltagePin 32
#define currentPin 33

#define readInterval 1000

EnergyMonitor emon;
AccumulationAverage<float> average;
AsyncWebServer server(80);
DataManager dataManager(emon, "/history.json");

void setup()
{
  Serial.begin(9600);

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("Power Meter");

  WiFiManager wifiManager;
  WiFiManagerParameter staticIpInput("ip", "Enter static IP address");
  WiFiManagerParameter gatewayInput("gateway", "Enter gateway address");
  wifiManager.addParameter(&staticIpInput);
  wifiManager.addParameter(&gatewayInput);
  wifiManager.autoConnect("Power Meter AP");


  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.js", "text/javascript");
  });

  server.on("/momentary", HTTP_GET, [](AsyncWebServerRequest *request){
    StaticJsonDocument<64> momentaryJson;
    emon.calcVI(20, 2000);
    momentaryJson["RealPower"] = emon.realPower;
    momentaryJson["ApparentPower"] = emon.apparentPower;
    momentaryJson["PowerFactor"] = emon.powerFactor;
    String buffer;
    serializeJson(momentaryJson, buffer);
    request->send(200, "application/json", buffer);
  });

  server.on("/history", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/history.json", "application/json");
  });

  server.begin();

  emon.voltage(voltagePin, 234.26, 1.7);
  emon.current(currentPin, 111.1);
}

void loop()
{
  dataManager.push();
}