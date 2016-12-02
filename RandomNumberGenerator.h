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
class BarycentricCoordinate;

enum RandomMethod {
    UnixRand
#ifdef __APPLE__
, Arc4Random
#endif
};

class RandomNumberGenerator
{
public:
	RandomNumberGenerator();
	~RandomNumberGenerator() {}
	
	inline float uniform01( void );
	inline float uniformRange( float min, float max );
    inline float cosineQuarterWave( void );

    void uniformUnitCircle( float & x, float & y );
    
	void uniformSurfaceUnitSphere( float & x, float & y, float & z );
    void uniformSurfaceUnitSphere( Vector4 & v );
    void uniformSurfaceUnitHalfSphere( const Vector4 & half_space, Vector4 & v );
    void uniformConeDirection( const Vector4 & dir, float angle, Vector4 & v );
    void cosineUnitHalfSphere( float & x, float & y, float & z );
    void cosineUnitHalfSphere( Vector4 & v );
    
    void uniformVolumeUnitSphere( float & x, float & y, float & z );

    void uniformUnitTriangle2D( float & x, float & y );
    void uniformTriangle3D( BarycentricCoordinate & bary );
    void uniformTriangle3D( const Vector4 & v1,
                            const Vector4 & v2,
                            const Vector4 & v3,
                            Vector4 & r );
    
	void seedCurrentTime();

private:
	inline float uniform01Impl( void );
    void buildCache();
	
    static const unsigned int CACHE_SIZE = 1000;

    unsigned int cache_next = CACHE_SIZE; // next available cache entry, or CACHE_SIZE if none left
    float cache[CACHE_SIZE];

    RandomMethod method = 
#ifdef __APPLE__
       Arc4Random;
#else
       UnixRand;
#endif
};

#include "RandomNumberGenerator.hpp"

#endif


