//
//  TMOctreeAccelerator.h
//  FastRender
//
//  Created by David Cunningham on 8/25/13.
//
//

#ifndef __FastRender__TMOctreeAccelerator__
#define __FastRender__TMOctreeAccelerator__

#include "TriangleMesh.h"
#include "AxisAlignedSlab.h"

class TMOctreeAccelerator : public TriangleMeshAccelerator
{
public:
    TMOctreeAccelerator( TriangleMesh & m );
    virtual ~TMOctreeAccelerator();
    
    class Node
    {
    public:
        Node();
        ~Node();

        void print( FILE * file = stdout, unsigned int level = 0 );

        // Helpers for intersection methods of TMOctreeAccelerator
        bool intersect( const Ray & ray, RayIntersection & intersection, TriangleMesh & mesh, unsigned int level ) const;
        bool intersectsAny( const Ray & ray, float min_distance, TriangleMesh & mesh, unsigned int level ) const;

        // FIXME - no need for bounds in each node. we can compute this knowing the depth in the tree and a global bounds
        AxisAlignedSlab bounds;
        // TODO - change this and the intersection code to use indices into a single triangle list in TriangleMesh
        std::vector< TriangleMesh::IndexTriangle >    triangles;
        // TODO perhaps speed this up with indices into a vector of Nodes rather than pointers
        // Pointers to child cells, coordinates indicated by:
        //      index % 2        -> 0:xmin 1:xmax
        //      (index >> 1) % 2 -> 0:ymin 1:ymax
        //      (index >> 2) % 2 -> 0:zmin 1:zmax
        Node * children[8];
    };
    
    Node root;
    
    virtual void build();
    void buildNode( Node * node );
	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;
    virtual bool intersectsAny( const Ray & ray, float min_distance ) const;
    virtual void print( FILE * file = stdout );
    
    // Coordinate indices are 0 or 1; returns index into children array
    inline unsigned char childIndex( unsigned char xi,
                                    unsigned char yi,
                                    unsigned char zi )
    { return zi << 2 | yi << 1 | xi; }

};

#endif /* defined(__FastRender__TMOctreeAccelerator__) */
