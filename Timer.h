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

class Timer {
public:
    Timer() : running(false), valid(false) {}
    ~Timer() {}
    
    void start();
    void stop();
    double elapsed();
    
    static double toDouble( const struct timeval & tm );
    
protected:
    
    struct timeval start_time;
    struct timeval end_time;
    bool running;
    bool valid;
    
};

#endif /* defined(__FastRender__Timer__) */
