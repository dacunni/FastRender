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


class Ray {
public:
	Ray() {}
	Ray( const Vector4 & o, const Vector4 & d ) : origin(o), direction(d) {}
	~Ray() {}
	
	Vector4 origin;
	Vector4 direction;
};

class RayIntersection {
public:
	RayIntersection() : best_hint(FLT_MAX) {}
	~RayIntersection() {}
	
	Ray ray;
	Vector4 position;
	Vector4 normal;
	float distance;
    float best_hint;
};



#endif
