#ifndef __TIMER__
#define __TIMER__

#include <sys/time.h>
#include <time.h>

double timeNowAsDouble();

class ProcessorTimer {
public:
    ProcessorTimer() = default;
    ~ProcessorTimer() = default;
    
    void start();
    void stop();
    double elapsed();
    
protected:
    clock_t start_time;
    clock_t end_time;
    bool running = false;
    bool valid   = false;
};

class WallClockTimer {
public:
    WallClockTimer() = default;
    ~WallClockTimer() = default;

    void start();
    void stop();
    double elapsed();

protected:
    struct timeval start_time;
    struct timeval end_time;
    bool running = false;
    bool valid   = false;
};

#endif
