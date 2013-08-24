//
//  RandomNumberGenerator.hpp
//  FastRender
//
//  Created by David Cunningham on 8/23/13.
//
//

#ifndef FastRender_RandomNumberGenerator_hpp
#define FastRender_RandomNumberGenerator_hpp

inline float RandomNumberGenerator::uniform01( void )
{
	// TODO - Use a better RNG
    const float one_over_RM = 1.0f / (float) RAND_MAX;
	return (float) rand() * one_over_RM;
}

inline float RandomNumberGenerator::uniformRange( float min, float max )
{
	return uniform01() * (max - min) + min;
}



#endif
