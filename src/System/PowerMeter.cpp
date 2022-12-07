#include "System/PowerMeter.h"

using namespace System;

PowerMeter::PowerMeter(std::string configPath)
{
    std::ifstream configFile(configPath);
    json configJson = json::parse(configFile);
    Logging::Logger = Logging::Log(Logging::Level::Silent, &std::clog);
}

Logging::Log configureLogger(const JsonResource& configResource)
{
    try
    {
        json configJson = configResource.deserialize();
        configJson.at("/logger/")       
    }
    catch(const std::exception& e)
    {
        Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << e.what() << std::endl;
    }
    catch(...)
    {
        Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << "Unexpected Exception" << std::endl;
    }
    return json();
}