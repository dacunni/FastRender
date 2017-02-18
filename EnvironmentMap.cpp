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
#include "RandomNumberGenerator.h"

bool EnvironmentMap::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    intersection.sample = sample( ray );
    intersection.ray = ray;
    intersection.distance = FLT_MAX;
    return true;
}

EnvironmentMap::ImportanceSample
EnvironmentMap::importanceSample( RandomNumberGenerator & rng,
                                          const RayIntersection & intersection )
{
    throw("Invalid call to importanceSample() on class without support");
    return ImportanceSample();
}

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

// FIXME: We should pass the a RNG in somewhere
RandomNumberGenerator temp_rng;

HDRImageEnvironmentMap::HDRImageEnvironmentMap( const std::string & filename,
                                                unsigned int w, unsigned int h)
    : image(filename, w, h)
{
    std::vector<float> pdf;
    image.toGray(pdf);
    sampler.reset(new DistributionSampler2D(temp_rng, &pdf[0], w, h));
}

RGBRadianceSample HDRImageEnvironmentMap::sample( const Ray & ray ) const
{
    // FIXME[DAC]: Why do we have to negate this? Does this work properly from all angles?
    const Vector4 & d = ray.direction;
    //const Vector4 d = ray.direction.negated();

    // u,v in [-1, 1]
    float r = (1.0f / M_PI) * acos(d.z) / sqrt( sq(d.x) + sq(d.y) );
    float u = d.x * r;
    float v = d.y * r;

    // Map u,v in [-1, 1] to [0, 1]
    u = (u + 1.0f) * 0.5f;
    v = (v + 1.0f) * 0.5f;

    //printf("S: u %f v %f\n", u, v); // TEMP

    return image.sampleRGB(u, v);
}

EnvironmentMap::ImportanceSample
HDRImageEnvironmentMap::importanceSample( RandomNumberGenerator & rng,
                                          const RayIntersection & intersection )
{
    DistributionSampler2D::Sample distSample;
    distSample = sampler->sample();

    ImportanceSample mapSample;
    mapSample.pdf = distSample.pdf;
    mapSample.ray.origin = intersection.position;
    // Map u,v in [0, 1] to [-1, 1]
    float u = distSample.u * 2.0f - 1.0f;
    float v = distSample.v * 2.0f - 1.0f;
    // u,v to direction - must be the inverse of sample()!
    float theta = atan2f(v, u);
    float phi = M_PI * sqrtf(u * u + v * v);
    mapSample.ray.direction.x = sinf(phi) * cos(theta);
    mapSample.ray.direction.y = sinf(phi) * sin(theta);
    mapSample.ray.direction.z = cosf(phi);

    // TODO: Handle sample pointing away from normal
    //       Currently the caller handles this

    //printf("D: u %f v %f phi %f theta %f pdf %f\n", distSample.u, distSample.v,
    //       phi, theta, distSample.pdf); // TEMP

    return mapSample;
}



