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
    
    std::vector< Vector4 >          vertices;
    std::vector< IndexTriangle >    triangles;
};


void makeTriangleMeshTetrahedron( TriangleMesh & mesh );

#endif /* defined(__FastRender__TriangleMesh__) */
