#ifndef __LIBGEEK_CORE_LOGGER_H_
#define __LIBGEEK_CORE_LOGGER_H_

#include <string>

namespace Geek {

enum LoggerLevel_t
{
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class Logger
{
 private:
    std::string m_name;

 public:
    Logger(std::string name);
    ~Logger();

    void log(LoggerLevel_t level, const char* __format, ...);
};

};

#endif
