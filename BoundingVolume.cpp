//
//  BoundingVolume.cpp
//  FastRender
//
//  Created by David Cunningham on 3/8/13.
//
//

#include "BoundingVolume.h"
#include "Ray.h"
#include "AxisAlignedSlab.h"

bool BoundingVolume::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    if( bound == nullptr || object == nullptr )
        return false;
    
    RayIntersection temp_isec = intersection;
    
    if( bound->intersect( ray, temp_isec ) ) {
        return object->intersect( ray, intersection );
    }
    
    return false;
}

bool BoundingVolume::intersectsAny( const Ray & ray, float min_distance ) const
{
    if( bound == nullptr || object == nullptr )
        return false;
        
    if( bound->intersectsAny( ray, min_distance ) ) {
        return object->intersectsAny( ray, min_distance );
    }
    
    return false;    
}


void BoundingVolume::buildAxisAligned( Traceable * o )
{
    if( !o )
        return;
    
    object = o;    
    bound = o->getAxisAlignedBounds();
}

void BoundingVolume::print( FILE * file ) const
{
    if( bound ) {
        printf("Bound:\n");
        bound->print( file );
    }
}


