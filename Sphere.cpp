/*
 *  Sphere.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 7/23/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <cmath>

#include "Sphere.h"

unsigned long Sphere::intersection_test_count = 0;


bool Sphere::intersect( const Ray & ray, RayIntersection & intersection ) const
{
	Vector4 dst;
	float b = 0, c = 0, d = 0;
	
    intersection_test_count++;
    
	subtract( ray.origin, center, dst );
	dot( dst, ray.direction, b );
	dot( dst, dst, c );
	c -= sq( radius );
	d = sq( b ) - c;

    // Fail early if this intersection is farther than the closest hit so far
    if( d > sq( intersection.best_hint ) )
        return false;
    
	if( d > intersection.min_distance ) {
		intersection.ray = ray;
		intersection.distance = -b - sqrt(d);
		// compute intersection position
		scale( ray.direction, intersection.distance, intersection.position );
		add( intersection.position, ray.origin, intersection.position );
		// compute surface normal
		subtract( intersection.position, center, intersection.normal );
		intersection.normal.normalize();
		return true;
	}
	else {
		return false;
	}
}
