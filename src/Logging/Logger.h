#ifndef LOGGER_H
#define LOGGER_H

#include <json.hpp>
#include <iostream>
#include <fstream>

namespace Logging
{
    enum class Level
    {
        Silent,
        Error,
        Warning,
        Info,
        Verbose
    };

    class NullStreamBuffer : public std::streambuf
    {};

    class Log
    {
    public:
        Log(Level level, std::ostream& stream);
        std::ostream& operator[](Level level);

    private:
        Level m_level;
        std::ostream& m_stream;
        NullStreamBuffer m_nullBuffer;
        std::ostream m_nullStream;
    };

    // Log DefaultLogger(Level::Verbose, std::cout);
}

#endif