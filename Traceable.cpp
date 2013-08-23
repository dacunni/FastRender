/*
 *  Traceable.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "Traceable.h"
#include "Ray.h"

bool Traceable::intersectsAny( const Ray & ray, float min_distance ) const
{
    RayIntersection intersection;
    intersection.min_distance = min_distance;
    return intersect( ray, intersection );
}
