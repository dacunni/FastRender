/*
 *  RandomNumberGenerator.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdlib.h>
#include <time.h>

#include "RandomNumberGenerator.h"

float RandomNumberGenerator::uniform01( void )
{
	// TODO - Use a better RNG
	return (float) rand() / RAND_MAX;
}

float RandomNumberGenerator::uniformRange( float min, float max )
{
	return uniform01() * (max - min) + min;
}


void RandomNumberGenerator::seedCurrentTime()
{
	srand( time( NULL ) );
}
