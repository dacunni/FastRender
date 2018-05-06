#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <ostream>
#include <string>
#include <sstream>

class Logger
{
    public:
        Logger() = default;
        virtual ~Logger() = default;

        enum Severity {
            Normal,
            Warning,
            Error,
            Fatal,
            Debug
        };
        static const std::string severityToString(Severity s, bool colorize = false);

        // Main log method. All others forms call this.
        virtual void log(Severity s, const std::string & msg);

        // simple string logging
        void normal(const std::string & msg)  { log(Normal, msg);  }
        void warning(const std::string & msg) { log(Warning, msg); }
        void error(const std::string & msg)   { log(Error, msg);   }
        void fatal(const std::string & msg)   { log(Fatal, msg);   }
        void debug(const std::string & msg)   { log(Debug, msg);   }

        // printf()-style logging
        void vlogf(Severity, const char * fmt, va_list args);
        void logf(Severity s, const char * fmt, ...);
        void normalf(const char * fmt, ...);
        void warningf(const char * fmt, ...);
        void errorf(const char * fmt, ...);
        void fatalf(const char * fmt, ...);
        void debugf(const char * fmt, ...);

        // Helper for ostream flavor of log methods (eg: logger.normal() << "message").
        // Log methods return an instance of this class, which accumulates a string
        // from chained operator<<()s. Finally, when the OStreamBuffer temporary goes out
        // of scope, it flushes the resulting message to the Logger that created it.
        struct OStreamBuffer {
            OStreamBuffer(Logger & l, Severity s) : logger(l), severity(s) {}
            OStreamBuffer(OStreamBuffer &) = delete;
            OStreamBuffer(OStreamBuffer &&) = default;
            ~OStreamBuffer() { logger.log(severity, ss.str()); }

            template<typename T> OStreamBuffer & operator<<(T v) { ss << v; return *this; }

            Logger & logger;
            Severity severity;
            std::stringstream ss;
        };

        // ostream-style logging
        OStreamBuffer normal()  { return OStreamBuffer(*this, Normal);  }
        OStreamBuffer warning() { return OStreamBuffer(*this, Warning); }
        OStreamBuffer error()   { return OStreamBuffer(*this, Error);   }
        OStreamBuffer fatal()   { return OStreamBuffer(*this, Fatal);   }
        OStreamBuffer debug()   { return OStreamBuffer(*this, Debug);   }

    protected:
        void logToStout(Severity s, const std::string & msg);
};

#endif
