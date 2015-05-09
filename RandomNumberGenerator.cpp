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
#include <assert.h>
#include <algorithm>

#include "RandomNumberGenerator.h"
#include "Transform.h"
#include "Vector.h"

using std::max;

RandomNumberGenerator::RandomNumberGenerator()
{
    seedCurrentTime();
}

void RandomNumberGenerator::seedCurrentTime()
{
    switch(method) {
        default:
        case UnixRand:
            srand( time( NULL ) );
            break;
        case Arc4Random:
            srand( time( NULL ) );
            arc4random_stir();
            break;
    }
}

// Generate a random 3D point on the surface of the unit sphere by means of the rejection method
void RandomNumberGenerator::uniformSurfaceUnitSphere( float & x, float & y, float & z )
{
    float u1 = uniform01();
    float u2 = uniform01();
    
    z = 1.0f - 2.0f * u1;
    float r = sqrtf( max( 0.0f, 1.0f - z * z) );
    float phi = 2.0f * M_PI * u2;
    x = r * cosf( phi );
    y = r * sinf( phi );
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

void RandomNumberGenerator::uniformConeDirection( const Vector4 & dir, float theta, Vector4 & v )
{
    // Find random points on the unit sphere within a code centered at the origin facing along +Z
    float cos_theta = cosf( theta );
    v.z = uniformRange( cos_theta, 1.0f );
    float phi = uniformRange( 0.0f, 2.0 * M_PI );
    float s = sqrt( 1.0f - sq(v.z) );
    v.x = s * cosf( phi );
    v.y = s * sinf( phi );

    // Rotate the point such that the center of the randomization is along the input vector 
    // FIXME - This certainly going to be be slow when called a lot.
    //       - Make just the rotation we need, not the inverse
    //       - Make a batch random number picker so we can only take the hit once
    // FIXME - this doesn't seem to work
    // FIXME - use the normalized direction vector
    static const Vector4 up( 0.0f, 0.0f, 1.0f );
    Transform xform = makeRotation( acos(dot(up, dir)), cross(up, dir) ); 
    v = mult( xform.fwd, v );
}

void RandomNumberGenerator::cosineUnitHalfSphere( float & x, float & y, float & z )
{
    float az = uniformRange( 0.0f, 2.0f * M_PI );
    // cosine-weighted pdf aligned to y axis
    float dec = cosineQuarterWave();
    x = cosf( az ) * sinf( dec );
    y = sinf( az ) * sinf( dec );
    z = cosf( dec );
}

void RandomNumberGenerator::cosineUnitHalfSphere( Vector4 & v )
{
    cosineUnitHalfSphere( v.x, v.y, v.z );
    v.w = 1.0;
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


