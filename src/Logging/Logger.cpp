#include "Logger.h"

using namespace Logging;

Log::Log(Level level, std::ostream&& stream) :
    m_level(level),
    m_stream
{}

std::ostream& Log::operator[](Level level)
{
    if(level <= m_level && level > Level::Silent)
        return m_stream;
    return m_stream;

}