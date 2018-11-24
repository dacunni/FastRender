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
    AxisAlignedSlab( const Vector4 & minCoord, const Vector4 & maxCoord );
    virtual ~AxisAlignedSlab() {}
    
    void correctMinMax( void );

    inline float xdim() const { return std::abs(xmax - xmin); }
    inline float ydim() const { return std::abs(ymax - ymin); }
    inline float zdim() const { return std::abs(zmax - zmin); }
    float maxdim() const;

    inline float xmid() const { return 0.5f * (xmin + xmax); }
    inline float ymid() const { return 0.5f * (ymin + ymax); }
    inline float zmid() const { return 0.5f * (zmin + zmax); }

    inline float volume() const { return xdim() * ydim() * zdim(); }
    
    virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;
	virtual bool intersectsAny( const Ray & ray, float min_distance ) const;

    float xmin, ymin, zmin;
    float xmax, ymax, zmax;

    static unsigned long intersection_test_count;       // Counts the number of intersection tests against
                                                        // objects of this class
    virtual void print( FILE * file = stdout ) const;
    virtual std::string toJSON() const;
    virtual std::string toString() const;
    virtual void visit( TraceableVisitor & visitor );

private:
    inline bool intersectHelper( const Ray & ray, RayIntersection & intersection, bool any_test ) const;
};

AxisAlignedSlab merge( const AxisAlignedSlab & a, const AxisAlignedSlab & b );

#endif /* defined(__FastRender__AxisAlignedSlab__) */
