/*
 *  Ray.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 7/23/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <typeinfo>
#include <sstream>

#include "Ray.h"
#include "Material.h"
#include "Scene.h"

std::string Ray::toString() const
{
    std::stringstream ss;
    ss << "ray"
       << " o: " << origin.toString()
       << " d: " << direction.toString();
    return ss.str();
}

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

Vector4 RayIntersection::fromDirection() const
{
    Vector4 from_dir = ray.direction.negated();

    // Asserts
    from_dir.assertIsUnity();
    from_dir.assertIsDirection();

    return from_dir;
}

void RayIntersection::print()
{
    printf("Ray Intersection :\n");
    printf("pos          : "); position.print();
    printf("normal       : "); normal.print();
    if(material) {
        printf("material     : %s\n", material->name().c_str());
    }
    else {
        printf("material     : null\n");
    }
    printf("sample       : "); sample.color.print();
    printf("distance     : %f\n", distance);
    printf("min_distance : %f\n", min_distance);

}
