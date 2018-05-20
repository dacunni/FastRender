//
//  TriangleMesh.cpp
//  FastRender
//
//  Created by David Cunningham on 8/10/13.
//
//

#include <sstream>

#include "TriangleMesh.h"
#include "Ray.h"
#include "AxisAlignedSlab.h"
#include "GeometryUtils.h"
#include "TraceableVisitor.h"

unsigned long TriangleMesh::intersection_test_count = 0;

TriangleMesh::TriangleMesh()
:   mesh_data(std::make_shared<MeshData>()),
    accelerator(nullptr)
{

}

TriangleMesh::~TriangleMesh()
{
    if( accelerator != nullptr )
        delete accelerator;
}

// Fit in unit cube centered at origin
void TriangleMesh::makeCanonical()
{
    std::shared_ptr<AxisAlignedSlab> bounds( getAxisAlignedBounds() );
    //printf("makeCanonical: Initial bounds: "); bounds->print();

    float maxdim = bounds->maxdim();

    // Shift to center around the origin
    float xshift = -bounds->xmid();
    float yshift = -bounds->ymid();
    float zshift = -bounds->zmid();

    // Scale to unit size along dimension with largest extent
    float scale = 1.0 / maxdim;

    auto & vertices = mesh_data->vertices;
    const unsigned int num_verts = vertices.size();
    for( unsigned int vi = 0; vi < num_verts; ++vi ) {
        vertices[vi][0] = (vertices[vi][0] + xshift) * scale;
        vertices[vi][1] = (vertices[vi][1] + yshift) * scale;
        vertices[vi][2] = (vertices[vi][2] + zshift) * scale;
    }

    bounds = getAxisAlignedBounds();
}

//
// Find the ray intersection with the triangles in the full mesh
//
bool TriangleMesh::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    intersection_test_count++;
    
    if( accelerator )
        return accelerator->intersect( ray, intersection );
    
    return intersectsTriangles( ray, mesh_data->triangles, intersection );
}

bool TriangleMesh::intersectsAny( const Ray & ray, float min_distance ) const
{
    if( accelerator )
        return accelerator->intersectsAny( ray, min_distance );
    
    RayIntersection intersection;
    intersection.min_distance = min_distance;
    return intersectsTriangles( ray, mesh_data->triangles, intersection, FAST_ISECT_TEST );
}

//
// Test for intersection of a ray with a single triangle
//
// This is an implementation of the Moller-Trumbore algorithm.
//
inline bool TriangleMesh::intersectsTriangle( const Ray & ray, const IndexTriangle & tri,
                                              float min_distance,
                                              float & t_param ) const
{
    Vector4 e1, e2;     // edge vectors
    Vector4 P, Q, T;
    const float epsilon = 1.0e-6;
    auto & vertices = mesh_data->vertices;

    // Compute edge vectors
    subtract( vertices[tri.vi[1]], vertices[tri.vi[0]], e1 );
    subtract( vertices[tri.vi[2]], vertices[tri.vi[0]], e2 );

    // Compute determinant
    cross( ray.direction, e2, P );
    float det = dot( e1, P );

    // If determinant zero, the ray does not intersect the plane of the triangle
    // Note, we're not culling backfaces.
    if( fabsf(det) < epsilon )
        return false;   // no intersection
    float inv_det = 1.0f / det;

    // Calculate vector from V0 to ray origin
    subtract( ray.origin, vertices[tri.vi[0]], T );

    // Calculate u coordinate and test whether the intersection lies within the valid range of u
    float u = inv_det * dot( T, P );
    if( u < 0.0f || u > 1.0f )
        return false;   // intersection out of valid u range

    // Calculate v coordinate and test whether the intersection lies within the valid range of v
    cross( T, e1, Q );
    float v = inv_det * dot( ray.direction, Q );
    if( v < 0.0f || u + v > 1.0f )
        return false;   // intersection out of u/v range

    float t = inv_det * dot( e2, Q );

    if( t > min_distance ) {
        t_param = t;
        return true;
    }

    return false;
}


//
// Find the ray intersection with the triangles in the supplied mesh
//
bool TriangleMesh::intersectsTriangles( const Ray & ray, const IndexTriangleArray & vtri,
                                        RayIntersection & intersection, IsectBehavior behavior ) const
{
    float t, best_t = FLT_MAX;
    bool hit = false;

    const IndexTriangle * best_tri;

    // Test for intersection against all triangles
    for( const IndexTriangle & tri : vtri ) {
        bool hit_tri = intersectsTriangle( ray, tri, intersection.min_distance, t );

        if( hit_tri ) {
            if( behavior == FAST_ISECT_TEST ) {
                return true;
            }
            else if( t < best_t ) {
                best_tri = &tri;
                best_t = t;
                hit = true;
            }
        }
    } // for

    if( hit ) {
        populateIntersection( ray, *best_tri, best_t, intersection );
    }
    
    return hit;
}

//
// Find the ray intersection with the triangles in the supplied mesh
// using the provided indices into the triangles list.
//
bool TriangleMesh::intersectsTriangles( const Ray & ray, const TriangleIndexArray & triangle_indices,
                                        RayIntersection & intersection, IsectBehavior behavior ) const
{
    auto & triangles = mesh_data->triangles;

    float t, best_t = FLT_MAX;
    bool hit = false;

    const IndexTriangle * best_tri;

    // Test for intersection against all triangles
    for( const unsigned int index : triangle_indices ) {
        const IndexTriangle & tri = triangles[index];

        bool hit_tri = intersectsTriangle( ray, tri, intersection.min_distance, t );

        if( hit_tri ) {
            if( behavior == FAST_ISECT_TEST ) {
                return true;
            }
            else if( t < best_t ) {
                best_tri = &tri;
                best_t = t;
                hit = true;
            }
        }
    } // for

    if( hit ) {
        populateIntersection( ray, *best_tri, best_t, intersection );
    }
    
    return hit;
}

inline void TriangleMesh::populateIntersection( const Ray & ray,
                                                const IndexTriangle & tri,
                                                float t,
                                                RayIntersection & intersection ) const
{
    auto & vertices = mesh_data->vertices;
    auto & normals = mesh_data->normals;
    auto & textureUVCoords = mesh_data->textureUVCoords;

    // Compute edge vectors
    // FIXME: This is redudant computation, but it is happening in another
    //        function. Should we just pass them around?
    Vector4 e1, e2;
    subtract( vertices[tri.vi[1]], vertices[tri.vi[0]], e1 );
    subtract( vertices[tri.vi[2]], vertices[tri.vi[0]], e2 );

    intersection.ray = ray;
    intersection.distance = t;
    // compute intersection position
    scale( ray.direction, intersection.distance, intersection.position );
    add( intersection.position, ray.origin, intersection.position );
#if 1
    // compute barycentric coordinate
    BarycentricCoordinate bary = barycentricForPointInTriangle( intersection.position, 
                                                                vertices[tri.vi[0]],
                                                                vertices[tri.vi[1]],
                                                                vertices[tri.vi[2]] );
    // Interpolate vertex normals
    intersection.normal = interpolate( normals[tri.vi[0]], normals[tri.vi[1]], normals[tri.vi[2]], bary );

    if( textureUVCoords.size() > 0 ) {
        intersection.u = textureUVCoords[tri.vi[0]].u * bary.u
                       + textureUVCoords[tri.vi[1]].u * bary.v
                       + textureUVCoords[tri.vi[2]].u * bary.w;
        intersection.v = textureUVCoords[tri.vi[0]].v * bary.u
                       + textureUVCoords[tri.vi[1]].v * bary.v
                       + textureUVCoords[tri.vi[2]].v * bary.w;
    }
#else
    // compute surface normal
    // TODO - make sure this normal agrees with front/back sense above
    intersection.normal = cross( e1, e2 ).normalized();
    if( textureUVCoords.size() > 0 ) {
        intersection.u = textureUVCoords[tri.vi[0]].u;
        intersection.v = textureUVCoords[tri.vi[0]].v;
    }
#endif

    if( !intersection.normal.isUnity() ) {
        // FIXME[DAC]: We shouldn't need to do this, but sometimes we get back bad normals
        //             from AssetLoader (due to another bug). For now, if we get a non-unity
        //             vector, we will simply fall back to calculating the normal as the
        //             perpendicular vector to the triangle.
        // TODO - make sure this normal agrees with front/back sense above
        intersection.normal = cross( e1, e2 ).normalized();
    }

    // Make sure the normal is pointing toward the direction of the incoming ray
    // TODO - move this to the general purpose ray intersection code so it will apply to any backwards normals
    if( dot( intersection.normal, ray.direction ) > 0.0f ) {
        intersection.normal.negate();
    }

    intersection.material = material;
    intersection.traceable = this;
}

std::shared_ptr<AxisAlignedSlab> TriangleMesh::getAxisAlignedBounds() const
{
    auto & vertices = mesh_data->vertices;

    if( vertices.empty() )
        return nullptr;
    
    auto bb = std::make_shared<AxisAlignedSlab>();
    
    bb->xmin = bb->xmax = vertices[0].x;
    bb->ymin = bb->ymax = vertices[0].y;
    bb->zmin = bb->zmax = vertices[0].z;
    
    for( auto vi : vertices ) {
        if( vi.x < bb->xmin ) bb->xmin = vi.x;
        if( vi.x > bb->xmax ) bb->xmax = vi.x;
        if( vi.y < bb->ymin ) bb->ymin = vi.y;
        if( vi.y > bb->ymax ) bb->ymax = vi.y;
        if( vi.z < bb->zmin ) bb->zmin = vi.z;
        if( vi.z > bb->zmax ) bb->zmax = vi.z;
    }
    
    return bb;
}

std::shared_ptr<TriangleMesh> combineMeshes( TriangleMeshArray & array )
{
    auto ubermesh = std::make_shared<TriangleMesh>();

    for(auto mesh : array) {
        auto & uber_data = *ubermesh->mesh_data;
        auto & part_data = *mesh->mesh_data;

        unsigned int part_vertex_start = uber_data.vertices.size();

        std::copy(part_data.vertices.begin(), part_data.vertices.end(),
                  std::back_inserter(uber_data.vertices));

        std::copy(part_data.normals.begin(), part_data.normals.end(),
                  std::back_inserter(uber_data.normals));

        std::transform(part_data.triangles.begin(), part_data.triangles.end(),
                       std::back_inserter(uber_data.triangles),
                       [&](TriangleMesh::IndexTriangle & tri) {
                           TriangleMesh::IndexTriangle offset_tri;
                           offset_tri.vi[0] = tri.vi[0] + part_vertex_start;
                           offset_tri.vi[1] = tri.vi[1] + part_vertex_start;
                           offset_tri.vi[2] = tri.vi[2] + part_vertex_start;
                           return offset_tri;
                       });
    }

    printf("Ubermesh vertices=%u normals=%u triangles=%u\n",
           (unsigned int) ubermesh->mesh_data->vertices.size(),
           (unsigned int) ubermesh->mesh_data->normals.size(),
           (unsigned int) ubermesh->mesh_data->triangles.size());

    return ubermesh;
}


void makeTriangleMeshTetrahedron( TriangleMesh & mesh )
{
    auto & vertices = mesh.mesh_data->vertices;
    auto & triangles = mesh.mesh_data->triangles;

    vertices.resize( 4 );
    triangles.resize( 4 );
    
    vertices[0] = Vector4( 0.0, 0.5, 0.0 );
    vertices[1] = Vector4( -0.5, -0.5, 0.0 );
    vertices[2] = Vector4( 0.5, -0.5, 0.0 );
    vertices[3] = Vector4( 0.0, 0.0, 1.0 );
    
    // TODO - make the winding order consistent
    triangles[0].vi[0] = 0;
    triangles[0].vi[1] = 1;
    triangles[0].vi[2] = 2;
    triangles[1].vi[0] = 1;
    triangles[1].vi[1] = 2;
    triangles[1].vi[2] = 3;
    triangles[2].vi[0] = 0;
    triangles[2].vi[1] = 1;
    triangles[2].vi[2] = 3;
    triangles[3].vi[0] = 0;
    triangles[3].vi[1] = 3;
    triangles[3].vi[2] = 2;
}

void makeTriangleMeshGroundPlatform( TriangleMesh & mesh, float size )
{
    auto & vertices = mesh.mesh_data->vertices;
    auto & triangles = mesh.mesh_data->triangles;

    vertices.resize( 4 );
    triangles.resize( 2 );
    
    float yoffset = -5.0; // TEMP
    
    vertices[0] = Vector4( -size / 2.0, yoffset, -size / 2.0 );
    vertices[1] = Vector4( -size / 2.0, yoffset, size / 2.0 );
    vertices[2] = Vector4( size / 2.0, yoffset, size / 2.0 );
    vertices[3] = Vector4( size / 2.0, yoffset, -size / 2.0 );
    
    // TODO - make the winding order consistent
    triangles[0].vi[0] = 0;
    triangles[0].vi[1] = 1;
    triangles[0].vi[2] = 2;
    triangles[1].vi[0] = 1;
    triangles[1].vi[1] = 3;
    triangles[1].vi[2] = 2;
}

std::string TriangleMesh::IndexTriangle::toJSON() const
{
    std::stringstream ss;
    
    ss << "["
        << vi[0] << ","
        << vi[1] << ","
        << vi[2] << "]";

    return ss.str();
}

std::string TriangleMesh::toString() const
{
    std::stringstream ss;

    ss << "trimesh"
       << " vertices: " << mesh_data->vertices.size()
       << " normals: " << mesh_data->normals.size()
       << " triangles: " << mesh_data->triangles.size();

    return ss.str();
}

std::string TriangleMesh::toJSON() const
{
    std::stringstream ss;

    ss << "{\"type\":\"TriangleMesh\",";

    ss << "\"vertices\":" << "[";
    for( auto vi = mesh_data->vertices.begin(); vi != mesh_data->vertices.end(); ++vi ) {
        if( vi != mesh_data->vertices.begin() ) { ss << ","; }
        ss << vi->toJSON();
    }
    ss << "],";
    ss << "\"normals\":" << "[";
    for( auto ni = mesh_data->normals.begin(); ni != mesh_data->normals.end(); ++ni ) {
        if( ni != mesh_data->normals.begin() ) { ss << ","; }
        ss << ni->toJSON();
    }
    ss << "],";
    ss << "\"triangles\":" << "[";
    for( auto ti = mesh_data->triangles.begin(); ti != mesh_data->triangles.end(); ++ti ) {
        if( ti != mesh_data->triangles.begin() ) { ss << ","; }
        ss << ti->toJSON();
    }
    ss << "]";

    ss << "}";

    return ss.str();
}

void TriangleMesh::visit( TraceableVisitor & visitor )
{
    visitor.handle(*this);
}

