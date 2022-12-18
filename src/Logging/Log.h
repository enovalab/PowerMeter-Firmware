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
    enum class Level
    {
        Silent,
        Error,
        Warning,
        Debug,
        Info,
        Verbose
    };

    class Log
    {
    public:
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

    Level getLevelByName(std::string name);
    std::string getNameByLevel(Level level);

    extern Log Logger;
}

#endif