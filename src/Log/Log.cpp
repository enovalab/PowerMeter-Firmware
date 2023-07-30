#include "Log.h"
#include <algorithm>
#include <exception>


Log Logger = Log(Level::Verbose);


Log::Log(Level level, std::ostream* stream, bool showLevel) noexcept :
    m_level(level),
    m_stream(stream),
    m_showLevel(showLevel)
{}


void Log::setLevel(Level level) noexcept
{
    m_level = level;
}


void Log::setOutputStream(std::ostream* stream) noexcept
{
    m_stream = stream;
}


void Log::setShowLevel(bool showLevel) noexcept
{
    m_showLevel = showLevel;
}


std::ostream& Log::operator[](Level level) noexcept
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



Level Log::getLevelByName(std::string name)
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
    if(name == "VERBOSE")
        return Level::Verbose;

    std::stringstream errorMessage;
    errorMessage << "Failed to convert \"" << name << "\" to 'Log::Level'";
    throw std::runtime_error(errorMessage.str());
}


std::string Log::getNameByLevel(Level level) noexcept
{
    switch(level)
    {
    case Level::Error:
        return "Error";
    case Level::Warning:
        return "Warning";
    case Level::Debug:
        return "Debug";
    case Level::Info:
        return "Info";
    case Level::Verbose:
        return "Verbose";
    }
    return "";
}