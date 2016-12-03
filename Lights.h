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
#include "Transform.h"

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

// TODO - Area lights should be Traceables, too
class AreaLight
{
public:
    AreaLight() = default;
    virtual ~AreaLight() = default;

    virtual Vector4 generateSample( RandomNumberGenerator & rng ) = 0;
    virtual Vector4 orientation() = 0;

    Transform transform;
    RGBColor band_power;
};

class CircleAreaLight : public AreaLight
{
public:
    //CircleAreaLight( const Vector4 & c, const Vector4 & d, float r )
    //    : center(c), direction(d), radius(r) {}
    CircleAreaLight() = default;
    virtual ~CircleAreaLight() = default;

    virtual Vector4 generateSample( RandomNumberGenerator & rng );
    virtual Vector4 orientation();

    //Vector4 center;
    //Vector4 direction;
    //float radius;
};



#endif
