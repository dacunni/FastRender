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
#include <math.h>

#include "RandomNumberGenerator.h"
#include "Vector.h"

RandomNumberGenerator::RandomNumberGenerator()
{
    seedCurrentTime();
}

void RandomNumberGenerator::seedCurrentTime()
{
	srand( time( NULL ) );
}

// Generate a random 3D point on the surface of the unit sphere by means of the rejection method
void RandomNumberGenerator::uniformSurfaceUnitSphere( float & x, float & y, float & z )
{
    float len_sq;
    
    do {
        x = uniformRange( -1.0, 1.0 );
        y = uniformRange( -1.0, 1.0 );
        z = uniformRange( -1.0, 1.0 );
    } while( (len_sq = x*x + y*y + z*z) > 1.0 );
    
    if( len_sq > 0.0 ) {
        float len = sqrtf( len_sq );
        float inv_len = 1.0f / len;
        x *= inv_len;
        y *= inv_len;
        z *= inv_len;
    }
}

void RandomNumberGenerator::uniformSurfaceUnitSphere( Vector4 & v )
{
    uniformSurfaceUnitSphere( v.x, v.y, v.z );
    v.w = 1.0;
}

void RandomNumberGenerator::uniformSurfaceUnitHalfSphere( const Vector4 & half_space, Vector4 & v )
{
    do {
        uniformSurfaceUnitSphere( v );
    } while( dot( v, half_space ) < 0.0 );
}

// Generate a random 3D point within the unit sphere by means of the rejection method
void RandomNumberGenerator::uniformVolumeUnitSphere( float & x, float & y, float & z )
{
    do {
        x = uniformRange( -1.0, 1.0 );
        y = uniformRange( -1.0, 1.0 );
        z = uniformRange( -1.0, 1.0 );
    } while( x*x + y*y + z*z > 1.0 );
}

void RandomNumberGenerator::buildCache()
{
    for( int i = 0; i < CACHE_SIZE; i++ ) {
        cache[i] = uniform01Impl();
    }
    cache_next = 0;
}


