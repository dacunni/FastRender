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
    BoundingVolume( std::shared_ptr<Traceable> o );
    virtual ~BoundingVolume() {}

    virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;
    virtual bool intersectsAny( const Ray & ray, float min_distance ) const;

    void buildAxisAligned( std::shared_ptr<Traceable> o );

    virtual std::shared_ptr<AxisAlignedSlab> getAxisAlignedBounds() const;

    virtual void print( FILE * file = stdout ) const;
    
    std::shared_ptr<Traceable> bound;
    std::shared_ptr<Traceable> object;
};

#endif /* defined(__FastRender__BoundingVolume__) */
