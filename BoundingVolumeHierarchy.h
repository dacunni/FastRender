//
//  BoundingVolumeHierarchy.h
//  FastRender
//
//  Created by David Cunningham on 4/5/16.
//
//

#ifndef __FastRender__BoundingVolumeHierarchy__
#define __FastRender__BoundingVolumeHierarchy__

#include <list>
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
    void buildBottomUp( std::shared_ptr<Container> container );
    void buildTopDown( std::shared_ptr<Container> container );
    std::shared_ptr<BoundingVolume> buildNodeTopDown( std::list<std::shared_ptr<BoundingVolume> > & objects );

    virtual void print( FILE * file = stdout ) const;
    virtual void visit( TraceableVisitor & visitor );

    std::shared_ptr<BoundingVolume> root = nullptr;
};

#endif
