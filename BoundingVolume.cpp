//
//  BoundingVolume.cpp
//  FastRender
//
//  Created by David Cunningham on 3/8/13.
//
//

#include <iostream>

#include "BoundingVolume.h"
#include "Ray.h"
#include "AxisAlignedSlab.h"

BoundingVolume::BoundingVolume( std::shared_ptr<Traceable> o )
{
    buildAxisAligned(o);
}

bool BoundingVolume::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    if( bound == nullptr || object == nullptr )
        return false;
    
    RayIntersection temp_isec = intersection;
    
    if( bound->intersect( ray, temp_isec ) ) {
        return object->intersectTransformed( ray, intersection );
    }
    
    return false;
}

bool BoundingVolume::intersectsAny( const Ray & ray, float min_distance ) const
{
    if( bound == nullptr || object == nullptr )
        return false;
        
    if( bound->intersectsAny( ray, min_distance ) ) {
        return object->intersectsAnyTransformed( ray, min_distance );
    }
    
    return false;    
}


void BoundingVolume::buildAxisAligned( std::shared_ptr<Traceable> o )
{
    if( !o ) {
        return;
    }
    
    object = o;    
    //bound = o->getAxisAlignedBounds();
    bound = o->getTransformedAxisAlignedBounds();
}

std::shared_ptr<AxisAlignedSlab> BoundingVolume::getAxisAlignedBounds() const
{
    return std::make_shared<AxisAlignedSlab>(*std::static_pointer_cast<AxisAlignedSlab>(bound)); 
}

void BoundingVolume::print( FILE * file ) const
{
    if( bound ) {
        fprintf(file, "Bound:\n");
        bound->print( file );
        if(object) {
            auto objTypeName = object->className();
            fprintf(file, "Object:\n");
            object->print();
        }
    }
}


