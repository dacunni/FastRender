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


#endif
