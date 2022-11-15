#include "WiFiManager.h"

using namespace Connectivity;

void WiFiManager::setupWiFi(const char* configFilePath)
{
    std::ifstream configFile(configFilePath);
    json config = json::parse(configFile);
    
}


void WiFiManager::createAP(const json& config)
{
    

}


void WiFiManager::connectSTA(const json& config)
{

}

