/*
 *  EnvironmentMap.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include <fstream>
#include <algorithm>

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

    if( fabsf( el - roundf(el) ) < tick1                     // 1 degree elevation
        || fabsf( el - 10.0f * roundf(0.1 * el) ) < tick10   // 10 degree elevation
        || fabsf( az - roundf(az) ) < tick1                  // 1 degree azimuth
        || fabsf( az - 10.0f * roundf(0.1 * az) ) < tick10 ) // 10 degree azimuth
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


HDRImageEnvironmentMap::HDRImageEnvironmentMap( const std::string & filename,
                                                unsigned int w, unsigned int h)
    : width(0),
      height(0)
{
    loadDataFromFile(filename, w, h);
}

RGBRadianceSample HDRImageEnvironmentMap::sample( const Ray & ray ) const
{
    // FIXME[DAC]: Why do we have to negate this? Does this work properly from all angles?
    const Vector4 & d = ray.direction;
    //const Vector4 d = ray.direction.negated();
    RGBRadianceSample s;
    s.mask = RGB_BITS;

    if( width == 0 || height == 0 ) {
        return s;
    }

    // u, v in [-1, 1]
    float r = (1.0f / M_PI) * acos(d.z) / sqrt( sq(d.x) + sq(d.y) );
    float u = d.x * r;
    float v = d.y * r;

    float row = (v + 1.0f) * 0.5f * height;
    float col = (u + 1.0f) * 0.5f * height;

    unsigned int ri = std::min( (unsigned int) row, height );
    unsigned int ci = std::min( (unsigned int) col, width );

    unsigned int offset = (ri * width + ci) * 3;

    //printf("ri = %u ci = %u\n", ri, ci); // TEMP
    
    // TODO[DAC]: Make the radiometry correct
    float scale_factor = 1.0;

    // TODO[DAC]: Interpolation
    s.color.r = data[ offset + 0 ] * scale_factor;
    s.color.g = data[ offset + 1 ] * scale_factor;
    s.color.b = data[ offset + 2 ] * scale_factor;

    return s;
}

void HDRImageEnvironmentMap::loadDataFromFile( const std::string & filename,
                                               unsigned int w, unsigned int h)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
    if( file.is_open() )
    {
        std::streampos size = file.tellg();
        assert( size == w * h * 3 * sizeof(float) );
        data.resize( size );
        file.seekg( 0, std::ios::beg );
        file.read( reinterpret_cast<char*>(&data[0]), size );
        file.close();
        width = w;
        height = h;
    }
    else {
        printf("Error reading data from %s\n", filename.c_str());
        width = 0;
        height = 0;
    }
}

