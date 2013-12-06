//
//  Timer.cpp
//  FastRender
//
//  Created by David Cunningham on 8/4/13.
//
//

#include <stdlib.h>
#include "Timer.h"

double Timer::toDouble( const struct timeval & tm ){
    return (double) tm.tv_sec + (double) tm.tv_usec * 1.0e-6;
}

void Timer::start()
{
    gettimeofday( &start_time, NULL );
    running = true;
    valid = true;
}

void Timer::stop()
{
    gettimeofday( &end_time, NULL );
    running = false;
}

double Timer::elapsed()
{
    if( !valid ) {
        return 0.0;
    }
    
    if( running ) {
        gettimeofday( &end_time, NULL );
    }
    
    return toDouble( end_time ) - toDouble( start_time );
}
