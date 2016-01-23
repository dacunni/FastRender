//
//  AxisAlignedSlab.h
//  FastRender
//
//  Created by David Cunningham on 3/8/13.
//
//

#ifndef __FastRender__AxisAlignedSlab__
#define __FastRender__AxisAlignedSlab__

#include "Traceable.h"

class AxisAlignedSlab : public Traceable
{
public:
    AxisAlignedSlab();
    AxisAlignedSlab( float xmin_default, float ymin_default, float zmin_default,
                     float xmax_default, float ymax_default, float zmax_default );
    AxisAlignedSlab( float xmin_default, float ymin_default, float zmin_default,
                     float cube_size );
    virtual ~AxisAlignedSlab() {}
    
    void correctMinMax( void );

    float xdim() const { return xmax - xmin; }
    float ydim() const { return ymax - ymin; }
    float zdim() const { return zmax - zmin; }
    float maxdim() const;
    
    virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;

    float xmin, ymin, zmin;
    float xmax, ymax, zmax;

    static unsigned long intersection_test_count;       // Counts the number of intersection tests against
                                                        // objects of this class
    virtual void print( FILE * file = stdout ) const;
};

#endif /* defined(__FastRender__AxisAlignedSlab__) */
