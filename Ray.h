/*
 *  Ray.h
 *  FastRender
 *
 *  Created by David Cunningham on 7/23/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _RAY_H_
#define _RAY_H_

#include <float.h>
#include <math.h>

#include "Vector.h"
#include "Color.h"

class Scene;
class Material;

class Ray {
public:
	Ray() : depth(1) {}
	Ray( const Vector4 & o, const Vector4 & d ) : origin(o), direction(d), depth(1) {}
	~Ray() {}
	
	Vector4 origin;
	Vector4 direction;
    unsigned char depth;
};

class RGBRadianceSample {
public:
    RGBRadianceSample() : color(0.0f, 0.0f, 0.0f), mask(0x0) {}
    ~RGBRadianceSample() {}

    RGBColor color;
    RGBColorMask mask;     // Which wavelength(s) are we sampling {RED_BIT, GREEN_BIT, BLUE_BIT}
};

class RayIntersection {
public:
	RayIntersection() : min_distance(0.0f), best_hint(FLT_MAX), material(NULL) {}
	~RayIntersection() {}
	
	Ray ray;
	Vector4 position;
	Vector4 normal;
    RGBRadianceSample sample;
    Material * material;
	float distance;
    float min_distance;
    float best_hint;
};

class Sample {
public:
    float value;
};

void trace( Ray & ray, Scene & scene, RayIntersection & intersection, Sample & sample );
void shade( Scene & scene, RayIntersection & intersection, Sample & sample );

#endif
