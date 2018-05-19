//
//  TMOctreeAccelerator.cpp
//  FastRender
//
//  Created by David Cunningham on 8/25/13.
//
//

#include <memory>
#include <algorithm>
#include "TMOctreeAccelerator.h"
#include "Ray.h"

// Uncomment to enable debug printfs
//#define DEBUG_BUILD_TREE

#ifdef DEBUG_BUILD_TREE
// TODO[DAC]: Put these in the instance so we can get stats for every octree separately
static int debug_depth = 0;
static int debug_max_depth = 0;
static int debug_num_inner_nodes = 0;
static int debug_num_leaves = 0;
static int debug_num_nodes = 0;
static float debug_average_depth = 0;
#endif

// Set to non-zero to break BVH decent at the specified level for debugging
#define DEBUG_VISUALIZE_HIERARCHY_LEVEL 0

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
#ifdef DEBUG_BUILD_TREE
    debug_max_depth = 0;
    debug_num_leaves = 0;
    debug_num_inner_nodes = 0;
    debug_num_nodes = 0;
    debug_average_depth = 0;
#endif
    auto full_bounds = mesh.getAxisAlignedBounds();
    root.bounds = *full_bounds;
    
#ifdef DEBUG_BUILD_TREE
    printf( "#### bb: %5f:%5f, %5f:%5f, %5f:%5f):\n",
            root.bounds.xmin, root.bounds.xmax,
            root.bounds.ymin, root.bounds.ymax,
            root.bounds.zmin, root.bounds.zmax
            );
#endif
    
    for( auto ti = 0; ti < mesh.mesh_data->triangles.size(); ++ti ) {
        root.triangles.push_back( ti );
    }
    
    buildNode( &root );
#ifdef DEBUG_BUILD_TREE
    debug_num_leaves = debug_num_nodes - debug_num_inner_nodes;
    debug_average_depth /= (float) debug_num_nodes;
    printf("TMOctreeAccelerator::build() summary:\n");
    printf("    max depth = %d\n", debug_max_depth);
    printf("    num leaves = %d\n", debug_num_leaves);
    printf("    num inner = %d\n", debug_num_inner_nodes);
    printf("    num nodes = %d\n", debug_num_nodes);
    printf("    avg depth = %f\n", debug_average_depth);
#endif
}

void TMOctreeAccelerator::buildNode( Node * node )
{
#ifdef DEBUG_BUILD_TREE
    debug_num_nodes++;
    debug_depth++;
    debug_average_depth += 1.0f;
    if( debug_depth > debug_max_depth ) {
        debug_max_depth = debug_depth;
    }
#endif
    bool has_children = false;
    
    // Don't subdivide beyond a preset limit
    const unsigned int max_triangles_per_node = 32;
    // Require that we're making at least this much progress per child node
    const float min_reduction_factor = 0.25;

    if( node->triangles.size() <= max_triangles_per_node ) {
#ifdef DEBUG_BUILD_TREE
        debug_depth--;
#endif
        return;
    }
    
    // Determine splitting planes
    float xsplit = (node->bounds.xmin + node->bounds.xmax) * 0.5f;
    float ysplit = (node->bounds.ymin + node->bounds.ymax) * 0.5f;
    float zsplit = (node->bounds.zmin + node->bounds.zmax) * 0.5f;
    
    // Bitfields to mark where each triangle falls
    enum { XLOW = 0x1, XHIGH = 0x2, YLOW = 0x4, YHIGH = 0x8, ZLOW = 0x10, ZHIGH = 0x20 };

    // Loop over all triangles, adding them to children where the overlap
    for( auto ti = 0; ti < node->triangles.size(); ++ti ) {
        unsigned char bins = 0x0;
        auto triangle_index = node->triangles[ ti ];
        TriangleMesh::IndexTriangle & tri = mesh.mesh_data->triangles[ triangle_index ];
        for( unsigned int vi = 0; vi < 3; vi++ ) {
            Vector4 & vertex = mesh.mesh_data->vertices[ tri.vi[ vi ] ];
            
            if( vertex.x <= xsplit ) bins |= XLOW;
            if( vertex.x >= xsplit ) bins |= XHIGH;
            if( vertex.y <= ysplit ) bins |= YLOW;
            if( vertex.y >= ysplit ) bins |= YHIGH;
            if( vertex.z <= zsplit ) bins |= ZLOW;
            if( vertex.z >= zsplit ) bins |= ZHIGH;
        }
        
        // Add triangle to child nodes based on which bits were set
        const unsigned char perms[8] = {
            XLOW  | YLOW  | ZLOW,
            XHIGH | YLOW  | ZLOW,
            XLOW  | YHIGH | ZLOW,
            XHIGH | YHIGH | ZLOW,
            XLOW  | YLOW  | ZHIGH,
            XHIGH | YLOW  | ZHIGH,
            XLOW  | YHIGH | ZHIGH,
            XHIGH | YHIGH | ZHIGH
        };
        for( unsigned int ci = 0; ci < 8; ci++ ) {
            if( !((bins & perms[ci]) ^ perms[ci]) ) {
#if 0
#ifdef DEBUG_BUILD_TREE
                printf("ti=%u ci=%u perm=0x%X\n", ti, ci, perm);
#endif
#endif
                if( !node->children[ ci ] ) {
                    node->children[ ci ] = new Node();
                    has_children = true;
                }
                node->children[ ci ]->triangles.push_back( triangle_index );
            }
        }
        
#if 0
#ifdef DEBUG_BUILD_TREE
        printf(">> ti %u : 0x%X (%u%u%u%u%u%u)\n",
               ti, (unsigned int) bins,
               !!(bins & XLOW), !!(bins & XHIGH),
               !!(bins & YLOW), !!(bins & YHIGH),
               !!(bins & ZLOW), !!(bins & ZHIGH));
#endif
#endif
    }

#ifdef DEBUG_BUILD_TREE
    printf("node triangles = %lu\n", node->triangles.size());
#endif
    
    if( has_children ) {
        // Make sure we aren't splitting without making progress by determining the
        // worst reduction we see amongst the child nodes.

        float worst_reduction = 1.0f;
        for( unsigned int ci = 0; ci < 8; ci++ ) {
            if( node->children[ci] ) {
                unsigned long child_size = (unsigned long) node->children[ci]->triangles.size();
                float reduction = 1.0f - (float) child_size / node->triangles.size();
#ifdef DEBUG_BUILD_TREE
                printf("children @ %u = %lu (%f)\n", ci, child_size, reduction);
#endif
                worst_reduction = std::min(reduction, worst_reduction);
            }
        }

        if( worst_reduction < min_reduction_factor )
        {
#ifdef DEBUG_BUILD_TREE
            printf("Aborting split due to insufficient reduction\n");
#endif
            for( unsigned int ci = 0; ci < 8; ci++ ) {
                if( node->children[ci] ) {
                    delete node->children[ci];
                    node->children[ci] = nullptr;
                }
            }
#ifdef DEBUG_BUILD_TREE
            debug_depth--;
#endif
            return;
        }

        node->triangles.clear(); // Inner nodes do not hold triangles
#ifdef DEBUG_BUILD_TREE
        debug_num_nodes++;
        debug_num_inner_nodes++;
#endif
    }
    
    for( unsigned int ci = 0; ci < 8; ci++ ) {
        if( node->children[ci] ) {
#ifdef DEBUG_BUILD_TREE
            printf("children @ %u = %lu\n", ci, (unsigned long) node->children[ci]->triangles.size());
#endif
            Node * child = node->children[ci];
            // Set the bounding box for the child node
            child->bounds = node->bounds;
            if( ci & XBIT ) child->bounds.xmin = xsplit;
            else            child->bounds.xmax = xsplit;
            if( ci & YBIT ) child->bounds.ymin = ysplit;
            else            child->bounds.ymax = ysplit;
            if( ci & ZBIT ) child->bounds.zmin = zsplit;
            else            child->bounds.zmax = zsplit;

            // Recursively build child
            buildNode( child );
        }
    }
#ifdef DEBUG_BUILD_TREE
    debug_depth--;
#endif
}


bool TMOctreeAccelerator::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    unsigned int indices[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    childOrderForDirection( ray.direction, indices );
    return root.intersect( ray, intersection, mesh, 0, indices );
}

bool TMOctreeAccelerator::intersectsAny( const Ray & ray, float min_distance ) const
{
    return root.intersectsAny( ray, min_distance, mesh, 0 );
}

void TMOctreeAccelerator::childOrderForDirection( const Vector4 & d, unsigned int indices[8] ) const
{
    unsigned int xsense = d.x < 0 ? 0 : 1;
    unsigned int ysense = d.y < 0 ? 0 : 1;
    unsigned int zsense = d.z < 0 ? 0 : 1;
    float xmag = fabsf(d.x);
    float ymag = fabsf(d.y);
    float zmag = fabsf(d.z);

    unsigned int xstep = 1, ystep = 1, zstep = 1;
    if( xmag > ymag ) xstep *= 2; else ystep *= 2;
    if( xmag > zmag ) xstep *= 2; else zstep *= 2;
    if( ymag > zmag ) ystep *= 2; else zstep *= 2;

    for( unsigned int ci = 0; ci < 8; ci++ ) {
        indices[ci] = 0;
        if( ((ci / xstep) & 0x1) == xsense )
            indices[ci] |= XBIT;
        if( ((ci / ystep) & 0x1) == ysense )
            indices[ci] |= YBIT;
        if( ((ci / zstep) & 0x1) == zsense )
            indices[ci] |= ZBIT;
    }
}

bool TMOctreeAccelerator::Node::intersect( const Ray & ray, RayIntersection & intersection,
                                           TriangleMesh & mesh, unsigned int level,
                                           unsigned int indices[8] ) const
{
    // Early rejection test for bounding box intersection
    if( !bounds.intersectsAny( ray, intersection.min_distance ) ) {
        return false;
    } 

#if DEBUG_VISUALIZE_HIERARCHY_LEVEL > 0
    if( (bounds.intersect( ray, intersection ) && level > DEBUG_VISUALIZE_HIERARCHY_LEVEL)) {
        return true;
    }
#endif

    bool isleaf = triangles.size() > 0;

    if( isleaf ) {
        // For leaf nodes, we check for intersections with the triangle list
        return mesh.intersectsTriangles( ray, triangles, intersection );
    }
    else {
        bool found_isect = false;

        // For inner nodes, we recurse to children to determine if there is an intersection.
        // Note, we don't check the triangles in non-leaf nodes, as they are restricted to not contain any triangles during construction
        for( unsigned int index = 0; index < 8; index++ ) {
            unsigned int ci = indices[index];
            if( children[ci] ) {
                if( children[ci]->intersect( ray, intersection, mesh, level + 1, indices ) ) {
                    found_isect = true;
                    break;
                }
            }
        }
        return found_isect;
    }
}

bool TMOctreeAccelerator::Node::intersectsAny( const Ray & ray, float min_distance,
                                               TriangleMesh & mesh, unsigned int level ) const
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
        // Note, we don't check the triangles in non-leaf nodes, as they are restricted to not
        // contain any triangles during construction
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
                         ((ci & XBIT) ? "XH" : "XL"),
                         ((ci & YBIT) ? "YH" : "YL"),
                         ((ci & ZBIT) ? "ZH" : "ZL") );
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

