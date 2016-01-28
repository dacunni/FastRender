//
//  Timer.h
//  FastRender
//
//  Created by David Cunningham on 8/4/13.
//
//

#ifndef __FastRender__Timer__
#define __FastRender__Timer__

#include <sys/time.h>
#include <time.h>

#define USE_PROCESS_TIMER 1

class Timer {
public:
    Timer() : running(false), valid(false) {}
    ~Timer() {}
    
    void start();
    void stop();
    double elapsed();
    
    static double toDouble( const struct timeval & tm );
    
protected:
    
#if USE_PROCESS_TIMER
    clock_t start_time;
    clock_t end_time;
#else
    struct timeval start_time;
    struct timeval end_time;
#endif
    bool running;
    bool valid;
    
};

#endif /* defined(__FastRender__Timer__) */
