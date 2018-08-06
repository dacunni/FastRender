#ifndef __TIMER__
#define __TIMER__

#include <sys/time.h>
#include <time.h>

double timeNowAsDouble();

class ProcessorTimer {
public:
    ProcessorTimer() : running(false), valid(false) {}
    ~ProcessorTimer() {}
    
    void start();
    void stop();
    double elapsed();
    
protected:
    clock_t start_time;
    clock_t end_time;
    bool running;
    bool valid;
};

class WallClockTimer {
public:
    WallClockTimer() : running(false), valid(false) {}
    ~WallClockTimer() {}
    void start();
    void stop();
    double elapsed();
protected:
    struct timeval start_time;
    struct timeval end_time;
    bool running;
    bool valid;
};

#endif
