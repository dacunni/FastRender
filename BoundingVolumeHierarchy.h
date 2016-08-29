//
//  BoundingVolumeHierarchy.h
//  FastRender
//
//  Created by David Cunningham on 4/5/16.
//
//

#ifndef __FastRender__BoundingVolumeHierarchy__
#define __FastRender__BoundingVolumeHierarchy__

#include "BoundingVolume.h"
#include "Container.h"

class BoundingVolumeHierarchy : public Traceable
{
public:
    BoundingVolumeHierarchy() {}
    virtual ~BoundingVolumeHierarchy() {}

    virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;
    virtual bool intersectsAny( const Ray & ray, float min_distance ) const;

    void build( std::shared_ptr<Container> container );

    virtual void print( FILE * file = stdout ) const;

    std::shared_ptr<BoundingVolume> root = nullptr;
};

#endif
