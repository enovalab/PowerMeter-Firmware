#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <sstream>

#define SOURCE_LOCATION                                     \
static_cast<std::ostringstream&&>(                          \
    std::ostringstream()                                    \
        << __FILE__                                         \
        << ":" << __LINE__                                  \
        << " in '" << __PRETTY_FUNCTION__ << "': "          \
).str().c_str()                                             \


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
    static std::string getNameByLevel(Level level) noexcept;

    Log(Level level, std::ostream* stream = &std::cout, bool showLevel = false) noexcept;
    void setLevel(Level level) noexcept;
    void setOutputStream(std::ostream* stream) noexcept;
    void setShowLevel(bool showLevel) noexcept;
    std::ostream& operator[](Level level) noexcept;

private:
    Level m_level;
    bool m_showLevel;
    std::ostream* m_stream;
};

extern Log Logger;

using Level = Log::Level;

#endif