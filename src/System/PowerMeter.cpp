#include "System/PowerMeter.h"

using namespace System;

PowerMeter::PowerMeter(std::string configPath)
{
    std::ifstream configFile(configPath);
    json configJson = json::parse(configFile);
    Logging::Logger = Logging::Log(Logging::Level::Silent, &std::clog);
}