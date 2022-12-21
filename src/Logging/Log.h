#ifndef LOGGING_LOG_H
#define LOGGING_LOG_H

#include <iostream>
#include <sstream>

#define SOURCE_LOCATION                                     \
static_cast<std::ostringstream&&>(                          \
    std::ostringstream()                                    \
        << __FILE__                                         \
        << ":" << __LINE__                                  \
        << " in '" << __PRETTY_FUNCTION__ << "': "          \
).str().c_str()                                             \


namespace Logging
{
    class Log
    {
    public:
        enum class Level
        {
            Silent,
            Error,
            Warning,
            Debug,
            Info,
            Verbose
        };

        static Level getLevelByName(std::string name);
        static std::string getNameByLevel(Level level);

        Log(Level level, std::ostream* stream, bool showLevel = false);
        void setLevel(Level level);
        void setOutputStream(std::ostream* stream);
        void setShowLevel(bool showLevel);
        std::ostream& operator[](Level level);

    private:
        Level m_level;
        bool m_showLevel;
        std::ostream* m_stream;
    };

    extern Log Logger;
}

using Level = Logging::Log::Level;

#endif