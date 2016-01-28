/*
 *  Traceable.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <typeinfo>

#include "Material.h"
#include "Traceable.h"
#include "Ray.h"

Traceable::Traceable()
    : material(NULL),
      transform(NULL)
{

}

Traceable::~Traceable()
{
    if( material )
        delete material;
    if( transform )
        delete transform;
}

bool Traceable::intersectsAny( const Ray & ray, float min_distance ) const
{
    RayIntersection intersection;
    intersection.min_distance = min_distance;
    return intersect( ray, intersection );
}

bool Traceable::intersectTransformed( const Ray & ray, RayIntersection & intersection ) const
{
    if( transform ) {
        Ray tray = ray;
        tray.origin = mult( transform->rev, ray.origin );
        // Fix direction W in case other code is sloppy
        Vector4 dir = ray.direction;
        dir.w = 0.0;
        tray.direction = mult( transform->rev, dir );
        tray.direction.normalize();
        bool hit = intersect( tray, intersection );
        if( hit ) {
            intersection.position = mult( transform->fwd, intersection.position );
            intersection.distance = subtract( intersection.position, ray.origin ).magnitude();
            // Set normal W to be 0, since vector math elsewhere may be sloppy about this
            intersection.normal.w = 0;
            intersection.normal = mult( transform->fwd, intersection.normal );
            // TODO - add strategic asserts to make sure normals have proper w=0
        }
        return hit;
    }
    else {
        return intersect( ray, intersection );
    }
}

bool Traceable::intersectsAnyTransformed( const Ray & ray, float min_distance ) const
{
    // FIXME - how should we adjust min_distance?
    if( transform ) {
        Ray tray = ray;
        tray.origin = mult( transform->rev, ray.origin );
        tray.direction = mult( transform->rev, ray.direction );
        tray.direction.normalize();
        return intersectsAny( tray, min_distance );
    }
    else {
        return intersectsAny( ray, min_distance );
    }
}

void Traceable::print( FILE * file ) const
{
    // FIXME - can we demangle this name?
    fprintf( file, "Traceable (%s) has_xform: %s\n",
             typeid(*this).name(),
             transform ? "yes" : "no" );
}




