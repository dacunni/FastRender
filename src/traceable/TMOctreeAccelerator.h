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

    enum { XBIT = 0x1, YBIT = 0x2, ZBIT = 0x4 };
    
    class Node
    {
    public:
        Node();
        ~Node();

        void print( FILE * file, unsigned int level,
                    const AxisAlignedSlab & bounds );

        // Helpers for intersection methods of TMOctreeAccelerator
        bool intersect( const Ray & ray, RayIntersection & intersection,
                        TriangleMesh & mesh,
                        const AxisAlignedSlab & bounds,
                        unsigned int level,
                        unsigned int indices[8] ) const;
        bool intersectsAny( const Ray & ray, float min_distance,
                            TriangleMesh & mesh,
                            const AxisAlignedSlab & bounds,
                            unsigned int level ) const;

        TriangleMesh::TriangleIndexArray    triangles;
        // TODO perhaps speed this up with indices into a vector of Nodes rather than pointers
        // Pointers to child cells, coordinates indicated by:
        //      index % 2        -> 0:xmin 1:xmax
        //      (index >> 1) % 2 -> 0:ymin 1:ymax
        //      (index >> 2) % 2 -> 0:zmin 1:zmax
        Node * children[8];
    };
    
    Node root;
    AxisAlignedSlab rootBounds;
    
    virtual void build();
    void buildNode( Node * node, const AxisAlignedSlab & bound );
	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;
    virtual bool intersectsAny( const Ray & ray, float min_distance ) const;
    virtual void print( FILE * file = stdout );

    static inline AxisAlignedSlab childBounds( const AxisAlignedSlab & parentBounds,
                                               unsigned int ci ) {
        AxisAlignedSlab bounds = parentBounds;

        // Determine splitting planes
        float xsplit = (bounds.xmin + bounds.xmax) * 0.5f;
        float ysplit = (bounds.ymin + bounds.ymax) * 0.5f;
        float zsplit = (bounds.zmin + bounds.zmax) * 0.5f;

        // Update bounds
        if( ci & XBIT ) bounds.xmin = xsplit;
        else            bounds.xmax = xsplit;
        if( ci & YBIT ) bounds.ymin = ysplit;
        else            bounds.ymax = ysplit;
        if( ci & ZBIT ) bounds.zmin = zsplit;
        else            bounds.zmax = zsplit;

        return bounds;
    }
    
    // Coordinate indices are 0 or 1; returns index into children array
    inline unsigned char childIndex( unsigned char xi,
                                    unsigned char yi,
                                    unsigned char zi )
    { return zi << 2 | yi << 1 | xi; }

private:
    void childOrderForDirection( const Vector4 & d, unsigned int indices[8] ) const;
};

void addOctreeAccelerator( TriangleMesh & m );

#endif /* defined(__FastRender__TMOctreeAccelerator__) */
