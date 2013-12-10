//
//  BoundingVolume.h
//  FastRender
//
//  Created by David Cunningham on 3/8/13.
//
//

#ifndef __FastRender__BoundingVolume__
#define __FastRender__BoundingVolume__

#include "Traceable.h"

class BoundingVolume : public Traceable
{
public:
    BoundingVolume() {}
    virtual ~BoundingVolume() {}

    virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;
    virtual bool intersectsAny( const Ray & ray, float min_distance ) const;

    void buildAxisAligned( Traceable * o );

    virtual void print( FILE * file = stdout ) const;
    
    Traceable * bound = nullptr;
    Traceable * object = nullptr;
};

#endif /* defined(__FastRender__BoundingVolume__) */
