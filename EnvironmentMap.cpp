/*
 *  EnvironmentMap.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "EnvironmentMap.h"
#include "GeometryUtils.h"

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


ArcLightEnvironmentMap::ArcLightEnvironmentMap()
{
    recalculateRadiance();
}

ArcLightEnvironmentMap::ArcLightEnvironmentMap( const Vector4 & d, float r )
    : light_dir( d.normalized() ), angular_radius( r )
{
    recalculateRadiance();
}

void ArcLightEnvironmentMap::recalculateRadiance()
{
    float cap_area = sphericalCapSurfaceArea( angular_radius );

    // TODO[DAC]: Check this for correctness
    radiance_per_sample = power / cap_area / M_PI;

    // printf( "power               = %f\n", power );
    // printf( "radiance_per_sample = %f\n", radiance_per_sample );
}

RGBRadianceSample ArcLightEnvironmentMap::sample( const Ray & ray ) const
{
    RGBRadianceSample s;
    s.mask = RGB_BITS;

    if( acos( dot( ray.direction, light_dir ) ) < angular_radius ) {
        // TODO: Make a power parameter and adjust for size of lit region
        s.color.r = s.color.g = s.color.b = radiance_per_sample;
    }
    else {
        s.color.r = s.color.g = s.color.b = 0.0f;
    }

    return s;
}


