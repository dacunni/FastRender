/*
 *  Sphere.h
 *  FastRender
 *
 *  Created by David Cunningham on 7/23/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Ray.h"
#include "Traceable.h"

class Sphere : public Traceable
{
public:
	Sphere() {}
	Sphere( const Vector4 & c, float r ) : center(c), radius(r) {}
	~Sphere() {}
	
	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;

	Vector4 center;
	float radius;
    
    static unsigned long intersection_test_count;       // Counts the number of intersection tests against
                                                        // objects of this class    
};




#endif
