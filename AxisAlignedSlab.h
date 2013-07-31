//
//  AxisAlignedSlab.h
//  FastRender
//
//  Created by David Cunningham on 3/8/13.
//
//

#ifndef __FastRender__AxisAlignedSlab__
#define __FastRender__AxisAlignedSlab__

#include <iostream>
#include "Traceable.h"

class AxisAlignedSlab : public Traceable
{
public:
    AxisAlignedSlab( float xmin_default, float ymin_default, float zmin_default,
                     float xmax_default, float ymax_default, float zmax_default );
    AxisAlignedSlab();
    virtual ~AxisAlignedSlab() {}
    
    virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;

    float xmin, ymin, zmin;
    float xmax, ymax, zmax;
};

#endif /* defined(__FastRender__AxisAlignedSlab__) */
