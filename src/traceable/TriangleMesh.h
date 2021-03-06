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
#include "Types.h"

class TriangleMeshAccelerator;

class TriangleMesh : public Traceable
{
public:
    // IndexTriangle represents a single triangle, whose
    // vertices are in the TriangleMesh's vertices array.
    // It stores indices into this array.
    struct IndexTriangle {
        std::string toJSON() const;
        unsigned int vi[3];
    };

    using VertexArray        = std::vector<Vector4>;
    using NormalArray        = std::vector<Vector4>;
    using IndexTriangleArray = std::vector<IndexTriangle>;

    // List of indices into the IndexTriangleArray
    using TriangleIndexArray = std::vector<unsigned int>;
    using TextureUVArray     = std::vector<float2>;
    
    TriangleMesh();
    virtual ~TriangleMesh();
    
    // Fit in unit cube centered at origin
    void makeCanonical();

	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const;    
    virtual bool intersectsAny( const Ray & ray, float min_distance ) const;
    
    inline bool intersectsTriangle( const Ray & ray, const IndexTriangle & tri,
                             float min_distance,
                             float & t ) const;

    bool intersectsTriangles( const Ray & ray, const IndexTriangleArray & vtri,
                              RayIntersection & intersection ) const;
    bool intersectsAnyTriangles( const Ray & ray, const IndexTriangleArray & vtri,
                                 RayIntersection & intersection ) const;
    bool intersectsTriangles( const Ray & ray, const TriangleIndexArray & triangle_indices,
                              RayIntersection & intersection ) const;
    bool intersectsAnyTriangles( const Ray & ray, const TriangleIndexArray & triangle_indices,
                                 RayIntersection & intersection ) const;

    inline void populateIntersection( const Ray & ray, const IndexTriangle & tri,
                                      float t, RayIntersection & intersection ) const;

    virtual std::shared_ptr<AxisAlignedSlab> getAxisAlignedBounds() const;

    virtual std::string toString() const;
    virtual std::string toJSON() const;
    virtual void visit( TraceableVisitor & visitor );

    struct MeshData {
        VertexArray        vertices;
        NormalArray        normals;
        IndexTriangleArray triangles;
        TextureUVArray     textureUVCoords;
    };
    std::shared_ptr<MeshData> mesh_data;

    TriangleMeshAccelerator * accelerator;              // Intersetion acceleration object (null if none)
    
    static unsigned long intersection_test_count;       // Counts the number of intersection tests against
                                                        // objects of this class
};

using TriangleMeshArray = std::vector<std::shared_ptr<TriangleMesh>>;

std::shared_ptr<TriangleMesh> combineMeshes( TriangleMeshArray & array );

void makeTriangleMeshTetrahedron( TriangleMesh & mesh );
void makeTriangleMeshGroundPlatform( TriangleMesh & mesh, float size );

class TriangleMeshAccelerator : public Traceable
{
public:
    TriangleMeshAccelerator( TriangleMesh & m ) : mesh(m) {}
    virtual ~TriangleMeshAccelerator() {}
    
    virtual void build() = 0;
	virtual bool intersect( const Ray & ray, RayIntersection & intersection ) const = 0;
    virtual bool intersectsAny( const Ray & ray, float min_distance ) const = 0;
    
    TriangleMesh                  & mesh;
};

#endif
