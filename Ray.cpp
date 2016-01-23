/*
 *  Ray.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 7/23/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <typeinfo>

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

void RayIntersection::print()
{
    printf("Ray Intersection :\n");
    printf("pos          : "); position.print();
    printf("normal       : "); normal.print();
    printf("material     : %s\n", (material ? typeid(material).name() : "null"));
    printf("sample       : "); sample.color.print();
    printf("distance     : %f\n", distance);
    printf("min_distance : %f\n", min_distance);
    printf("best_hint    : %f\n", best_hint);

}
