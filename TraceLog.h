/*
 *  TraceLog.h
 *  FastRender
 *
 *  Created by David Cunningham on 12/12/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TRACE_LOG_H_
#define _TRACE_LOG_H_

#include <vector>
#include <ostream>
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
