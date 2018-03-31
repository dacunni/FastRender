#include <stdio.h>
#include <stdlib.h>
#include "Timer.h"

static double toDouble( const struct timeval & tm ){
    return (double) tm.tv_sec + (double) tm.tv_usec * 1.0e-6;
}

void ProcessorTimer::start()
{
    start_time = clock();
    running = true;
    valid = true;
}

void ProcessorTimer::stop()
{
    end_time = clock();
    running = false;
}

double ProcessorTimer::elapsed()
{
    if( !valid ) {
        return 0.0;
    }
    
    if( running ) {
        end_time = clock();
    }
    
    return (double) (end_time - start_time) / CLOCKS_PER_SEC;
}

void WallClockTimer::start()
{
    gettimeofday( &start_time, NULL );
    running = true;
    valid = true;
}

void WallClockTimer::stop()
{
    gettimeofday( &end_time, NULL );
    running = false;
}

double WallClockTimer::elapsed()
{
    if( !valid ) {
        return 0.0;
    }
    
    if( running ) {
        gettimeofday( &end_time, NULL );
    }
    
    return toDouble( end_time ) - toDouble( start_time );
}
