/*
 *  Lights.h
 *  FastRender
 *
 *  Created by David Cunningham on 11/28/16
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _LIGHTS_H_
#define _LIGHTS_H_

#include "Color.h"
#include "Vector.h"
#include "Traceable.h"

class RandomNumberGenerator;

class PointLight
{
public:
    PointLight() {}
    PointLight( const Vector4 & pos, const RGBColor & bp )
        : position(pos), band_power(bp) {}
    ~PointLight() {}

    Vector4  position;
    RGBColor band_power;
};

// TODO: Figure out how to handle transforms, as they impact the power density
//       output of the light. For now, only going to allow rotation / translation

struct LightSample {
    Vector4 position;
    Vector4 normal;
};

// Abstract base class for area lights
//   For simplicity, area lights are oriented with their front face pointing
//   in the +Y direction. Placement and orientation should be done with a Transform.
//   NOTE: Area lights should not be scaled
class AreaLight : public Traceable
{
public:
    AreaLight() {}
    AreaLight( const RGBColor & emittance );
    virtual ~AreaLight();

    virtual bool isAreaLight() const { return true; }

    // Return a point sampled uniformly across the surface
    virtual LightSample sampleSurface( RandomNumberGenerator & rng ) const = 0;
    LightSample sampleSurfaceTransformed( RandomNumberGenerator & rng ) const;

    virtual float area() const = 0;
};

class CircleAreaLight : public AreaLight
{
public:
    CircleAreaLight();
    CircleAreaLight( float r, const RGBColor & emittance );
    virtual ~CircleAreaLight();

	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;
    
    virtual LightSample sampleSurface( RandomNumberGenerator & rng ) const;

    virtual float area() const;

    float radius;
};


#endif
