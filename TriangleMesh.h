//
//  TriangleMesh.h
//  FastRender
//
//  Created by David Cunningham on 8/10/13.
//
//

#ifndef __FastRender__TriangleMesh__
#define __FastRender__TriangleMesh__

#include <vector>
#include "Traceable.h"
#include "Vector.h"

class TriangleMesh : public Traceable
{
public:
    class IndexTriangle {
    public:
        unsigned vi[3];
    };
    
	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;    
    virtual bool intersectsAny( const Ray & ray, float min_distance ) const;
    
    // Helper for intersect() and related methods
    enum IsectBehavior { CLOSEST_ISECT, FAST_ISECT_TEST };
    bool intersectsTriangles( const Ray & ray, const std::vector< IndexTriangle > & vtri,
                              RayIntersection & intersection, IsectBehavior behavior = CLOSEST_ISECT ) const;

    virtual AxisAlignedSlab * getAxisAlignedBounds() const;

    std::vector< Vector4 >          vertices;
    std::vector< IndexTriangle >    triangles;

    static unsigned long intersection_test_count;       // Counts the number of intersection tests against
                                                        // objects of this class
};


void makeTriangleMeshTetrahedron( TriangleMesh & mesh );
void makeTriangleMeshGroundPlatform( TriangleMesh & mesh, float size );

#endif /* defined(__FastRender__TriangleMesh__) */
