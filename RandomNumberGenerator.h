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
	~RandomNumberGenerator() = default;
	
	inline float uniform01( void );
	inline float uniformRange( float min, float max );
    inline float cosineQuarterWave( void );

    void uniformUnitCircle( float & x, float & y );
    void uniformCircle( float radius, float & x, float & y );
    
	void uniformSurfaceUnitSphere( float & x, float & y, float & z );
    void uniformSurfaceUnitSphere( Vector4 & v );
    void uniformSurfaceUnitHalfSphere( const Vector4 & half_space, Vector4 & v );
    void uniformConeDirection( const Vector4 & dir, float angle, Vector4 & v );

    // Distribution sampling
    //   Convention:
    //      +z out of surface
    //      phi measured CCW about z, with phi=0 is x+ axis
    //      theta measured from +z axis, with theta=0 is z+ axis
    void cosineUnitHalfSphere( float & x, float & y, float & z );
    void cosineUnitHalfSphere( Vector4 & v );
    void beckmanNDF( float roughness, float & x, float & y, float & z );
    void beckmanNDF( float roughness, Vector4 & v );
    
    void uniformVolumeUnitCube( float & x, float & y, float & z );
    void uniformVolumeUnitCube( Vector4 & v );

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


