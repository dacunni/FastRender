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

class Vector4;

class RandomNumberGenerator 
{
public:
	RandomNumberGenerator();
	~RandomNumberGenerator() {}
	
	inline float uniform01( void );
	inline float uniformRange( float min, float max );
    
	void uniformSurfaceUnitSphere( float & x, float & y, float & z );
    void uniformSurfaceUnitSphere( Vector4 & v );
    void uniformSurfaceUnitHalfSphere( const Vector4 & half_space, Vector4 & v );
    
    void uniformVolumeUnitSphere( float & x, float & y, float & z );
    
	void seedCurrentTime();
	
};

#include "RandomNumberGenerator.hpp"

#endif


