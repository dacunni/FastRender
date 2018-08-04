#ifndef FastRender_RandomNumberGenerator_hpp
#define FastRender_RandomNumberGenerator_hpp

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

inline float RandomNumberGenerator::uniform01( void )
{
    return distribution(engine);
}

inline float RandomNumberGenerator::uniformRange( float min, float max )
{
	return uniform01() * (max - min) + min;
}

inline float RandomNumberGenerator::cosineQuarterWave( void )
{
    float a = uniform01();
    return asinf(a);
}


#endif
