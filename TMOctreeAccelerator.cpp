//
//  TMOctreeAccelerator.cpp
//  FastRender
//
//  Created by David Cunningham on 8/25/13.
//
//

#include <memory>
#include "TMOctreeAccelerator.h"
#include "Ray.h"

// Uncomment to enable debug printfs
//#define DEBUG_BUILD_TREE

TMOctreeAccelerator::TMOctreeAccelerator( TriangleMesh & m )
:   TriangleMeshAccelerator(m)
{
    
}

TMOctreeAccelerator::~TMOctreeAccelerator()
{
    
}

TMOctreeAccelerator::Node::Node()
{
    for( int i = 0; i < 8; ++i )
        children[ i ] = nullptr;
}

TMOctreeAccelerator::Node::~Node()
{
    for( int i = 0; i < 8; ++i ) {
        if( children[ i ] ) {
            delete children[ i ];
        }
    }
}

void TMOctreeAccelerator::build()
{
    AxisAlignedSlab * full_bounds = mesh.getAxisAlignedBounds();
    root.bounds = *full_bounds;
    delete full_bounds;
    
#ifdef DEBUG_BUILD_TREE
    printf( "#### bb: %5f:%5f, %5f:%5f, %5f:%5f):\n",
            root.bounds.xmin, root.bounds.xmax,
            root.bounds.ymin, root.bounds.ymax,
            root.bounds.zmin, root.bounds.zmax
            );
#endif
    
    for( unsigned int ti = 0; ti < mesh.triangles.size(); ++ti ) {
        root.triangles.push_back( mesh.triangles[ ti ] );
    }
    
    buildNode( &root );
}

void TMOctreeAccelerator::buildNode( Node * node )
{
    bool has_children = false;
    
    // Don't subdivide beyond a preset limit
    const unsigned int max_triangles_per_node = 32;
    if( node->triangles.size() <= max_triangles_per_node )
        return;
    
    // Determine splitting planes
    float xsplit = (node->bounds.xmin + node->bounds.xmax) * 0.5f;
    float ysplit = (node->bounds.ymin + node->bounds.ymax) * 0.5f;
    float zsplit = (node->bounds.zmin + node->bounds.zmax) * 0.5f;
    
    // Bitfields to mark where each triangle falls
    enum { XLOW = 0x1, XHIGH = 0x2, YLOW = 0x4, YHIGH = 0x8, ZLOW = 0x10, ZHIGH = 0x20 };

    // Loop over all triangles, adding them to children where the overlap
    for( unsigned int ti = 0; ti < node->triangles.size(); ++ti ) {
        unsigned char bins = 0x0;
        TriangleMesh::IndexTriangle & tri = node->triangles[ ti ];
        for( unsigned int vi = 0; vi < 3; vi++ ) {
            Vector4 & vertex = mesh.vertices[ tri.vi[ vi ] ];
            
            if( vertex.x <= xsplit ) bins |= XLOW;
            if( vertex.x >= xsplit ) bins |= XHIGH;
            if( vertex.y <= ysplit ) bins |= YLOW;
            if( vertex.y >= ysplit ) bins |= YHIGH;
            if( vertex.z <= zsplit ) bins |= ZLOW;
            if( vertex.z >= zsplit ) bins |= ZHIGH;
        }
        
        // Add triangle to child nodes based on which bits were set
        unsigned char xb, yb, zb, perm;
        for( unsigned int ci = 0; ci < 8; ci++ ) {
            xb = (ci & 0x1) ? XHIGH : XLOW;
            yb = (ci & 0x2) ? YHIGH : YLOW;
            zb = (ci & 0x4) ? ZHIGH : ZLOW;
            perm = xb | yb | zb;
            if( !((bins & perm) ^ perm) ) {
#ifdef DEBUG_BUILD_TREE
                printf("ti=%u ci=%u perm=0x%X\n", ti, ci, perm);
#endif
                if( !node->children[ ci ] ) {
#ifdef DEBUG_BUILD_TREE
                    printf("N\n");
#endif
                    node->children[ ci ] = new Node();
                    has_children = true;
                }
                node->children[ ci ]->triangles.push_back( tri );
            }
        }
        
#ifdef DEBUG_BUILD_TREE
        printf(">> ti %u : 0x%X\n", ti, (unsigned int) bins );
#endif
    }

    // TODO - make sure we aren't splitting without making progress
    
    if( has_children ) {
        node->triangles.clear(); // Inner nodes do not hold triangles
    }
    
    for( unsigned int ci = 0; ci < 8; ci++ ) {
        if( node->children[ci] ) {
#ifdef DEBUG_BUILD_TREE
            printf("children @ %u = %lu\n", ci, (unsigned long) node->children[ci]->triangles.size());
#endif
            Node * child = node->children[ci];
            // Set the bounding box for the child node
            child->bounds = node->bounds;
            if( ci & 0x1 )
                child->bounds.xmin = xsplit;
            else
                child->bounds.xmax = xsplit;
            if( ci & 0x2 )
                child->bounds.ymin = ysplit;
            else
                child->bounds.ymax = ysplit;
            if( ci & 0x4 )
                child->bounds.zmin = zsplit;
            else
                child->bounds.zmax = zsplit;

            // Recursively build child
            buildNode( child );
        }
    }
}


bool TMOctreeAccelerator::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    return root.intersect( ray, intersection, mesh, 0 );
}

bool TMOctreeAccelerator::intersectsAny( const Ray & ray, float min_distance ) const
{
    return root.intersectsAny( ray, min_distance, mesh, 0 );
}

bool TMOctreeAccelerator::Node::intersect( const Ray & ray, RayIntersection & intersection, TriangleMesh & mesh, unsigned int level ) const
{
    bool isleaf = triangles.size() > 0;

    // Early rejection test for bounding box intersection
    if( !bounds.intersectsAny( ray, intersection.min_distance ) ) {
        return false;
    } 

#if 0
    // TEMP >>> - viz the bounding volume
    if( (bounds.intersect( ray, intersection ) && level > 4)) {
        return true;
    }
    // TEMP <<<
#endif

    if( isleaf ) {
        // For leaf nodes, we check for intersections with the triangle list
        return mesh.intersectsTriangles( ray, triangles, intersection );
    }
    else {
        bool found_isect = false;
        // For inner nodes, we recurse to children to determine if there is an intersection.
        // Note, we don't check the triangles in non-leaf nodes, as they are restricted to not contain any triangles during construction
        for( unsigned int ci = 0; ci < 8; ci++ ) {
            if( children[ci] ) {
                if( children[ci]->intersect( ray, intersection, mesh, level + 1 ) ) {
                    found_isect = true;
                }
            }
        }
        return found_isect;
    }
}

bool TMOctreeAccelerator::Node::intersectsAny( const Ray & ray, float min_distance, TriangleMesh & mesh, unsigned int level ) const
{
    bool isleaf = triangles.size() > 0;

    // Early rejection test for bounding box intersection
    if( !bounds.intersectsAny( ray, min_distance ) ) {
        return false;
    } 

    if( isleaf ) {
        // For leaf nodes, we check for intersections with the triangle list
        RayIntersection isect;
        isect.min_distance = min_distance;
        return mesh.intersectsTriangles( ray, triangles, isect, TriangleMesh::FAST_ISECT_TEST );
    }
    else {
        bool found_isect = false;
        // For inner nodes, we recurse to children to determine if there is an intersection.
        // Note, we don't check the triangles in non-leaf nodes, as they are restricted to not contain any triangles during construction
        for( unsigned int ci = 0; ci < 8; ci++ ) {
            if( children[ci] ) {
                if( children[ci]->intersectsAny( ray, min_distance, mesh, level + 1 ) ) {
                    return true;
                }
            }
        }
    }

    return false;
}


void TMOctreeAccelerator::Node::print( FILE * file, unsigned int level )
{
    bool isleaf = triangles.size() > 0;
    std::string prefix( level, '\t' );
    fprintf( file, "%sNode - level: %2u, type: %s, tri: %4lu, bb: %+5f:%+5f, %+5f:%+5f, %+5f:%+5f\n",
            prefix.c_str(), level, isleaf ? "LEAF" : "INNER",
            (unsigned long) triangles.size(),
            bounds.xmin, bounds.xmax,
            bounds.ymin, bounds.ymax,
            bounds.zmin, bounds.zmax
            );
    if( !isleaf ) {
        for( unsigned int ci = 0; ci < 8; ci++ ) {
            if( children[ci] ) {
                fprintf( file, "%s children[ %u ] (%s, %s, %s):\n", prefix.c_str(), ci,
                        ((ci & 0x1) ? "XH" : "XL"),
                        ((ci & 0x2) ? "YH" : "YL"),
                        ((ci & 0x4) ? "ZH" : "ZL") );
                children[ci]->print( file, level + 1 );
            }
        }
    }
}

void TMOctreeAccelerator::print( FILE * file )
{
    fprintf( file, "Triangle Mesh Octree Accelerator:\n" );
    root.print( file );
}

