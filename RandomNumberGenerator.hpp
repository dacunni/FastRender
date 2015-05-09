//
//  RandomNumberGenerator.hpp
//  FastRender
//
//  Created by David Cunningham on 8/23/13.
//
//

#ifndef FastRender_RandomNumberGenerator_hpp
#define FastRender_RandomNumberGenerator_hpp

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

inline float RandomNumberGenerator::uniform01Impl( void )
{
    const float one_over_RM = 1.0f / (float) RAND_MAX;

    switch(method) {
        default:
        case UnixRand:
            return (float) rand() * one_over_RM;
            break;
        case Arc4Random:
            u_int32_t r = arc4random_uniform(RAND_MAX + 1);
            return (float) r * one_over_RM;
            break;
    }
}

inline float RandomNumberGenerator::uniform01( void )
{
    if( cache_next >= CACHE_SIZE ) {
        buildCache();
    }
    
    return cache[ cache_next++ ];
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
