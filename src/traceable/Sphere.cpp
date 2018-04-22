/*
 *  Sphere.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 7/23/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <cmath>
#include <sstream>

#include "Sphere.h"
#include "TraceableVisitor.h"

unsigned long Sphere::intersection_test_count = 0;

bool Sphere::intersectHelper( const Ray & ray, float & dist1, float & dist2 ) const
{
	auto dst = subtract( ray.origin, center );
	float b = dot( dst, ray.direction );
	float c = dot( dst, dst ) - sq( radius );
	float d = sq( b ) - c;
    
    if( d < 0.0f )
        return false; // not hit

    float sqrtd = sqrtf(d);
    dist1 = -b - sqrtd;
    dist2 = -b + sqrtd;

    return true;
}

bool Sphere::intersectsAny( const Ray & ray, float min_distance ) const
{
    float dist1, dist2;
    if( !intersectHelper( ray, dist1, dist2 ) )
        return false;

    return dist1 >= min_distance ||
           dist2 >= min_distance;
}

bool Sphere::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    intersection_test_count++;

    float dist1, dist2;
    if( !intersectHelper( ray, dist1, dist2 ) )
        return false;

    if( dist2 < dist1 )
        std::swap( dist1, dist2 );  // TODO - is this necessary?
    
    if( dist2 < intersection.min_distance ) {
        return false;
    }
    if( dist1 < intersection.min_distance ) {
        dist1 = dist2;
    }
    
    intersection.ray = ray;
    intersection.distance = dist1;
    // compute intersection position
    intersection.position = add( ray.origin, scale( ray.direction, intersection.distance ) );
    // compute surface normal
    intersection.normal = subtract( intersection.position, center ).normalized();
    intersection.material = material;
    intersection.traceable = this;
    return true;
}

std::string Sphere::toString() const
{
    std::stringstream ss;
    ss << "sphere"
       << " c: " << center.toString()
       << " r: " << radius;
    return ss.str();
}

std::string Sphere::toJSON() const
{
    std::stringstream ss;

    ss << "{\"type\":\"Sphere\","
        << "\"center\":" << center.toJSON() << ","
        << "\"radius\":" << radius
        << "}";

    return ss.str();
}

void Sphere::visit( TraceableVisitor & visitor )
{
    visitor.handle(*this);
}

