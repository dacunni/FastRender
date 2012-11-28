/*
 *  RandomNumberGenerator.h
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _RANDOM_NUMBER_GENERATOR_H_
#define _RANDOM_NUMBER_GENERATOR_H_

class RandomNumberGenerator 
{
public:
	RandomNumberGenerator() {}
	~RandomNumberGenerator() {}
	
	float uniform01( void );
	float uniformRange( float min, float max );
	void uniformSurfaceUnitSphere( float & x, float & y, float & z );
    void uniformVolumeUnitSphere( float & x, float & y, float & z );
    
	void seedCurrentTime();
	
};



#endif


