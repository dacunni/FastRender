/*
 *  EnvironmentMap.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "EnvironmentMap.h"

RGBRadianceSample TestPatternEnvironmentMap::sample( const Ray & ray ) const
{
    RGBRadianceSample s;
    s.mask = RGB_BITS;

    float el = 90.0f * asinf( ray.direction.z ) / (M_PI / 2.0f);
    float az = 90.0f * atanf( ray.direction.y / ray.direction.x ) / (M_PI / 2.0f);

    //s.color.r = s.color.g = s.color.b = 0.75f;
    s.color.r = s.color.g = s.color.b = 1.0f;

    const float tick1 = 0.01f;
    const float tick10 = 0.05f;

    if( fabs( el - roundf(el) ) < tick1                     // 1 degree elevation
        || fabs( el - 10.0f * roundf(0.1 * el) ) < tick10   // 10 degree elevation
        || fabs( az - roundf(az) ) < tick1                  // 1 degree azimuth
        || fabs( az - 10.0f * roundf(0.1 * az) ) < tick10 ) // 10 degree azimuth
    {
        s.color.r = s.color.g = s.color.b = 0.0f;
    }

    return s;
}


RGBRadianceSample ArcLightEnvironmentMap::sample( const Ray & ray ) const
{
    RGBRadianceSample s;
    s.mask = RGB_BITS;

    // TODO - move to members. make a power parameter
    Vector4 light_dir = Vector4( 1.0, 1.0, 0.0 ).normalized();
    float angular_size = 0.2;

    if( acos( dot( ray.direction, light_dir ) ) / (M_PI / 2.0f) < angular_size ) {
        s.color.r = s.color.g = s.color.b = 30.0f;
    }
    else {
        s.color.r = s.color.g = s.color.b = 0.0f;
    }

    return s;
}


