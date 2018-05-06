#include <iostream>
#include <cstdio>
#include "Logger.h"

const std::string Logger::severityToString(Severity s)
{
    switch(s) {
        case Normal: return "Normal"; break;
        case Warning: return "Warning"; break;
        case Error: return "Error"; break;
        case Fatal: return "Fatal"; break;
        case Debug: return "Debug"; break;
        default: return "Unknown";
    }
}

void Logger::logToStout(Logger::Severity s, const std::string & msg)
{
    std::cout << severityToString(s) << "\t" << msg << std::endl;
}

void Logger::log(Logger::Severity s, const std::string & msg)
{
    logToStout(s, msg);
}

void Logger::vlogf(Severity s, const char * fmt, va_list args)
{
    const auto maxSize = 256;
    char buf[maxSize]; buf[0] = '\0';
    vsnprintf(buf, maxSize, fmt, args);
    log(s, buf);
}

void Logger::logf(Severity s, const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vlogf(s, fmt, args);
    va_end(args);
}

#define DFN_VARARG_LOG(NAME, SEVERITY) \
void Logger::NAME(const char * fmt, ...) { \
    va_list args;           \
    va_start(args, fmt);    \
    vlogf(SEVERITY, fmt, args);    \
    va_end(args);           \
}

DFN_VARARG_LOG(normalf, Normal);
DFN_VARARG_LOG(warningf, Warning);
DFN_VARARG_LOG(errorf, Error);
DFN_VARARG_LOG(fatalf, Fatal);
DFN_VARARG_LOG(debugf, Debug);


