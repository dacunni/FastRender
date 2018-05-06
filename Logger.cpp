#include <iostream>
#include <cstdio>
#include <unistd.h>
#include "Logger.h"

// ANSI terminal emulator color control codes
static const char * const ansi_red     = "\x1b[31m";
static const char * const ansi_green   = "\x1b[32m";
static const char * const ansi_yellow  = "\x1b[33m";
static const char * const ansi_blue    = "\x1b[34m";
static const char * const ansi_magenta = "\x1b[35m";
static const char * const ansi_cyan    = "\x1b[36m";
static const char * const ansi_reset   = "\x1b[0m";

const std::string Logger::severityToString(Severity s, bool colorize)
{
    if(colorize) {
        std::string level = severityToString(s, false);
        switch(s) {
            case Normal:  return ansi_green + level + ansi_reset; break;
            case Warning: return ansi_yellow + level + ansi_reset; break;
            case Error:   return ansi_red + level + ansi_reset; break;
            case Fatal:   return ansi_red + level + ansi_reset; break;
            case Debug:   return ansi_cyan + level + ansi_reset; break;
            default:      return level;
        }
    }
    switch(s) {
        case Normal:  return "Normal"; break;
        case Warning: return "Warning"; break;
        case Error:   return "Error"; break;
        case Fatal:   return "Fatal"; break;
        case Debug:   return "Debug"; break;
        default:      return "Unknown";
    }
}

void Logger::logToStout(Logger::Severity s, const std::string & msg)
{
    const bool loggingToTerminal = isatty(STDOUT_FILENO);
    std::cout<< severityToString(s, loggingToTerminal) << "\t" << msg << std::endl;
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


