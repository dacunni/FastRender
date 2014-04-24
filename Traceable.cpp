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
    : material(NULL)
{

}

Traceable::~Traceable()
{
    if( material )
        delete material;
}

bool Traceable::intersectsAny( const Ray & ray, float min_distance ) const
{
    RayIntersection intersection;
    intersection.min_distance = min_distance;
    return intersect( ray, intersection );
}

void Traceable::print( FILE * file ) const
{
    // FIXME - can we demangle this name?
    fprintf( file, "Traceable (%s)\n", typeid(*this).name() );
}
