#include "Tracker.h"
#include "Logging/Logger.h"
#include <json.hpp>
#include <fstream>

using namespace Measuring;

Tracker::Tracker(const Time::Clock& clock, std::string filePath, time_t interval, size_t samplesPerInterval) : 
    m_clock(clock),
    m_filePath(filePath),
    m_interval(interval),
    m_samplesPerInterval(samplesPerInterval)
{}

Tracker& Tracker::operator<<(float newValue)
{
    average << newValue;
    time_t now = m_clock.now();
    if(now - m_lastSample >= m_interval / m_samplesPerInterval)
    {
        pushBack(average);
        average.reset();
        m_lastSample = now;
    }
    return *this;
}

void Tracker::pushBack(float newValue)
{
    try
    {
        std::fstream jsonFile("/spiffs" + m_filePath, std::ios::out);
        json trackerArray = json::parse(jsonFile);
        trackerArray.push_back(newValue);
        trackerArray.erase(trackerArray.begin());
        jsonFile << trackerArray.dump();
    }
    catch(const std::exception& e)
    {
        // Logger::Log.errorln(e.what());
    }
    catch(...)
    {
        // Logger::Log.errorln(F("Unexpected exception occurred"));
    }
}