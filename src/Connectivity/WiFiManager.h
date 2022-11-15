#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <json.hpp>
#include <fstream>
#include <string>

namespace Connectivity
{
    namespace WiFiManager
    {
        void setupWiFi(const char* configFilePath)
        void createAP(const json& config);
        void connectSTA(const json& config);
    }
}

#endif