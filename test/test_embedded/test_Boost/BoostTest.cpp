#include <Arduino.h>
#include <SPIFFS.h>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <json.hpp>

using namespace boost::filesystem;

void setup()
{
    Serial.begin(115200);
    
    if(!SPIFFS.begin())
        std::cout << "SPIFFS failed" << std::endl;
        
    path filePath = "/spiffs/data/config/Logger.json";
    std::ifstream file(filePath.string());
    json fileData = json::parse(file);

    std::cout << fileData.dump(1) << std::endl;
}

void loop()
{}