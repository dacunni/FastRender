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

// TODO: Make area lights Traceables
// TODO: Figure out how to handle transforms, as they impact the power density
//       output of the light

// Abstrat base class for area lights
class AreaLight
{
public:
    AreaLight() {}
    AreaLight( const Vector4 & pos, const Vector4 & dir, const RGBColor & bp )
        : position(pos), direction(dir), band_power(bp) {}
    virtual ~AreaLight();

    Vector4 position;
    Vector4 direction;
    RGBColor band_power;
};

class CircleAreaLight : public AreaLight
{
public:
    CircleAreaLight() {}
    CircleAreaLight( const Vector4 & pos, const Vector4 & dir, float r, const RGBColor & bp )
        : AreaLight(pos, dir, bp), radius(r) {}

    float radius;
};


#endif
