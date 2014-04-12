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
    
    if( d < 0.0f )
        return false;

    float sqrtd = sqrtf(d);
    float dist1 = -b - sqrtd;
    float dist2 = -b + sqrtd;
    
    if( dist2 < dist1 )
        std::swap( dist1, dist2 );  // TODO - is this necessary?
    
    if( dist1 < intersection.min_distance ) {
        if( dist2 < intersection.min_distance ) {
            return false;
        }
        else {
            dist1 = dist2;
        }
    }
    
    // Fail early if this intersection is farther than the closest hit so far
    if( dist1 > intersection.best_hint ) {
        return false;
    }

    intersection.ray = ray;
    intersection.distance = dist1;
    // compute intersection position
    scale( ray.direction, intersection.distance, intersection.position );
    add( intersection.position, ray.origin, intersection.position );
    // compute surface normal
    subtract( intersection.position, center, intersection.normal );
    intersection.normal.normalize();
    intersection.material = material;
    return true;
}
