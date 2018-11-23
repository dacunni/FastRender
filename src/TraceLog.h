#ifndef _TRACE_LOG_H_
#define _TRACE_LOG_H_

#include <vector>
#include <iosfwd>
#include "Ray.h"

class Traceable;

class TraceLog
{
    public:
        class Entry
        {
            public:
                Ray ray;
                RayIntersection intersection;
                bool hit;

                void writeCSV( std::ostream & os );
                void writeRaw( std::ostream & os );
        };

        TraceLog() = default;
        virtual ~TraceLog() = default;

        void writeCSV( std::ostream & os );
        void writeCSV( const std::string & filename );

        void writeRaw( std::ostream & os );
        void writeRaw( const std::string & filename );

        std::vector<Entry> entries;
};


#endif
