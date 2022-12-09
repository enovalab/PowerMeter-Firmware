#include "Logging/Log.h"
#include <algorithm>

using namespace Logging;


Level Logging::getLevelByName(std::string name)
{
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);
    if(name == "SILENT")
        return Level::Silent;
    if(name == "ERROR")
        return Level::Error;
    if(name == "WARNING")
        return Level::Warning;
    if(name == "DEBUG")
        return Level::Debug;
    if(name == "INFO")
        return Level::Info;

    return Level::Verbose;
}


std::string Logging::getNameByLevel(Level level)
{
    switch(level)
    {
    case Level::Error:
        return "Error";
        break;
    case Level::Warning:
        return "Warning";
        break;
    case Level::Debug:
        return "Debug";
        break;
    case Level::Info:
        return "Info";
        break;
    case Level::Verbose:
        return "Verbose";
        break;
    default:
        return "";
    }
}


Log::Log(Level level, std::ostream* stream, bool showLevel) :
    m_level(level),
    m_stream(stream),
    m_showLevel(showLevel)
{}


std::ostream& Log::operator[](Level level)
{
    if(level <= m_level && level > Level::Silent)
    {
        if(m_showLevel)
        {
            std::string levelName = getNameByLevel(level);
            std::transform(levelName.begin(), levelName.end(), levelName.begin(), ::toupper);
            *m_stream << "[" << levelName << "] ";
        }
        return *m_stream;
    }

    class NullStreamBuffer : public std::streambuf
    {
    public:
        int overflow(int c) override { return c; }
    };

    static NullStreamBuffer nullBuffer;
    static std::ostream nullStream(&nullBuffer);
    return nullStream;
}