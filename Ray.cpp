/*
 *  Ray.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 7/23/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "Ray.h"
#include "Scene.h"


void trace( Ray & ray, Scene & scene, RayIntersection & intersection, Sample & sample )
{
    bool hit = scene.intersect( ray, intersection );

    if( hit ) {
        shade( scene, intersection, sample );
    }
}

void shade( Scene & scene, RayIntersection & intersection, Sample & sample )
{
    sample.value = intersection.normal.x;
}
