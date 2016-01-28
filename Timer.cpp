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
#if USE_PROCESS_TIMER
    start_time = clock();
#else
    gettimeofday( &start_time, NULL );
#endif
    running = true;
    valid = true;
}

void Timer::stop()
{
#if USE_PROCESS_TIMER
    end_time = clock();
#else
    gettimeofday( &end_time, NULL );
#endif
    running = false;
}

double Timer::elapsed()
{
    if( !valid ) {
        return 0.0;
    }
    
    if( running ) {
#if USE_PROCESS_TIMER
        end_time = clock();
#else
        gettimeofday( &end_time, NULL );
#endif
    }
    
#if USE_PROCESS_TIMER
    return (double) (end_time - start_time) / CLOCKS_PER_SEC;
#else
    return toDouble( end_time ) - toDouble( start_time );
#endif
}
