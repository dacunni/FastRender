/*
 *  Boolean.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 11/15/15.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <cmath>

#include "Boolean.h"

unsigned long Boolean::intersection_test_count = 0;


bool Boolean::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    return false;
}

bool BooleanDifference::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    RayIntersection left_isect;
    RayIntersection right_isect;

    bool left_hit = left->intersect( ray, left_isect );
    bool right_hit = left->intersect( ray, right_isect );

    if( !left_hit ) {
        return false;
    }

    if( left_hit && right_hit && left_isect.distance < right_isect.distance ) {
        intersection = left_isect;
        return true;
    }

    // TODO - step along the ray until we hit left while not in right or
    //        exit right while in left

    return false;
}

