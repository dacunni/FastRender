//
//  TriangleMesh.cpp
//  FastRender
//
//  Created by David Cunningham on 8/10/13.
//
//

#include "TriangleMesh.h"
#include "Ray.h"
#include "AxisAlignedSlab.h"

unsigned long TriangleMesh::intersection_test_count = 0;

TriangleMesh::TriangleMesh()
:   accelerator(nullptr)
{

}

TriangleMesh::~TriangleMesh()
{
    if( accelerator != nullptr )
        delete accelerator;
}

//
// Find the ray intersection with the triangles in the full mesh
//
bool TriangleMesh::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    intersection_test_count++;
    
    if( accelerator )
        return accelerator->intersect( ray, intersection );
    
    return intersectsTriangles( ray, triangles, intersection );
}

bool TriangleMesh::intersectsAny( const Ray & ray, float min_distance ) const
{
    if( accelerator )
        return accelerator->intersectsAny( ray, min_distance );
    
    RayIntersection intersection;
    intersection.min_distance = min_distance;
    return intersectsTriangles( ray, triangles, intersection, FAST_ISECT_TEST );
}

//
// Find the ray intersection with the triangles in the supplied mesh
//
// This is an implementation of the Moller-Trumbore algorithm.
//
bool TriangleMesh::intersectsTriangles( const Ray & ray, const std::vector< IndexTriangle > & vtri,
                                        RayIntersection & intersection, IsectBehavior behavior ) const
{
    Vector4 e1, e2;     // edge vectors
    Vector4 P, Q, T;
    float det, inv_det, u, v, t;
    float best_t = intersection.best_hint;
    bool hit = false;
    const float epsilon = 0.000001;
    
    //
    // Test for intersection against all triangles
    //
    for( const IndexTriangle & tri : vtri ) {
        //
        // Test triangle for intersection with the ray
        //
        
        // Compute edge vectors
        subtract( vertices[tri.vi[1]], vertices[tri.vi[0]], e1 );
        subtract( vertices[tri.vi[2]], vertices[tri.vi[0]], e2 );
        
        // Compute determinant
        cross( ray.direction, e2, P );
        det = dot( e1, P );
        
        // If determinant zero, the ray does not intersect the plane of the triangle
        // Note, we're not culling backfaces.
        if( det > -epsilon && det < epsilon )
            continue;   // no intersection
        inv_det = 1.0f / det;
        
        // Calculate vector from V0 to ray origin
        subtract( ray.origin, vertices[tri.vi[0]], T );
        
        // Calculate u coordinate and test whether the intersection lies within the valid range of u
        u = inv_det * dot( T, P );
        if( u < 0.0f || u > 1.0f )
            continue;   // intersection out of valid u range
        
        // Calculate v coordinate and test whether the intersection lies within the valid range of v
        cross( T, e1, Q );
        v = inv_det * dot( ray.direction, Q );
        if( v < 0.0f || u + v > 1.0f )
            continue;   // intersection out of u/v range
        
        t = inv_det * dot( e2, Q );
        
        // If this is a fast intersection test (ie: predicate, itersects or doesn't), return on first hit
        if( t > intersection.min_distance && behavior == FAST_ISECT_TEST ) {
            return true;
        }

        if( t > intersection.min_distance && t < best_t ) {
            intersection.ray = ray;
            intersection.distance = t;
            // compute intersection position
            scale( ray.direction, intersection.distance, intersection.position );
            add( intersection.position, ray.origin, intersection.position );
            // compute surface normal
            // TODO - make sure this normal agrees with front/back sense above
            cross( e1, e2, intersection.normal );
            best_t = t;
            intersection.best_hint = best_t;
            hit = true;
        }
        
    }
    
    // TODO - move this to the general purpose ray intersection code so it will apply to any backwards normals
    if( hit && dot( ray.direction, intersection.normal ) > 0.0 ) {
        intersection.normal.negate();
    }
    
    intersection.normal.normalize();

    if( hit ) {
        intersection.material = material;
    }
    
    return hit;
}


AxisAlignedSlab * TriangleMesh::getAxisAlignedBounds() const
{
    if( vertices.empty() )
        return nullptr;
    
    auto bb = new AxisAlignedSlab();
    
    bb->xmin = bb->xmax = vertices[0].x;
    bb->ymin = bb->ymax = vertices[0].y;
    bb->zmin = bb->zmax = vertices[0].z;
    
    for( auto vi : vertices ) {
        if( vi.x < bb->xmin )
            bb->xmin = vi.x;
        if( vi.x > bb->xmax )
            bb->xmax = vi.x;
        if( vi.y < bb->ymin )
            bb->ymin = vi.y;
        if( vi.y > bb->ymax )
            bb->ymax = vi.y;
        if( vi.z < bb->zmin )
            bb->zmin = vi.z;
        if( vi.z > bb->zmax )
            bb->zmax = vi.z;
    }
    
    return bb;
}


void makeTriangleMeshTetrahedron( TriangleMesh & mesh )
{
    mesh.vertices.resize( 4 );
    mesh.triangles.resize( 4 );
    
    float zoffset = -7.0; // TEMP
    
    mesh.vertices[0] = Vector4( 0.0, 0.5, 0.0 + zoffset );
    mesh.vertices[1] = Vector4( -0.5, -0.5, 0.0 + zoffset );
    mesh.vertices[2] = Vector4( 0.5, -0.5, 0.0 + zoffset );
    mesh.vertices[3] = Vector4( 0.0, 0.0, 1.0 + zoffset );
    
    // TODO - make the winding order consistent
    mesh.triangles[0].vi[0] = 0;
    mesh.triangles[0].vi[1] = 1;
    mesh.triangles[0].vi[2] = 2;
    mesh.triangles[1].vi[0] = 1;
    mesh.triangles[1].vi[1] = 2;
    mesh.triangles[1].vi[2] = 3;
    mesh.triangles[2].vi[0] = 0;
    mesh.triangles[2].vi[1] = 1;
    mesh.triangles[2].vi[2] = 3;
    mesh.triangles[3].vi[0] = 0;
    mesh.triangles[3].vi[1] = 3;
    mesh.triangles[3].vi[2] = 2;
}

void makeTriangleMeshGroundPlatform( TriangleMesh & mesh, float size )
{
    mesh.vertices.resize( 4 );
    mesh.triangles.resize( 2 );
    
    float yoffset = -5.0; // TEMP
    
    mesh.vertices[0] = Vector4( -size / 2.0, yoffset, -size / 2.0 );
    mesh.vertices[1] = Vector4( -size / 2.0, yoffset, size / 2.0 );
    mesh.vertices[2] = Vector4( size / 2.0, yoffset, size / 2.0 );
    mesh.vertices[3] = Vector4( size / 2.0, yoffset, -size / 2.0 );
    
    // TODO - make the winding order consistent
    mesh.triangles[0].vi[0] = 0;
    mesh.triangles[0].vi[1] = 1;
    mesh.triangles[0].vi[2] = 2;
    mesh.triangles[1].vi[0] = 1;
    mesh.triangles[1].vi[1] = 3;
    mesh.triangles[1].vi[2] = 2;
}

