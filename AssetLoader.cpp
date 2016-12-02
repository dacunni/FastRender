//
//  AssetLoader.cpp
//  FastRender
//
//  Created by David Cunningham on 8/15/13.
//
//

#include <algorithm>

// Using the Open Asset Import Library ("assimp" - http://assimp.sourceforge.net/ )
//#include <assimp/assimp.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include "AssetLoader.h"
#include "TriangleMesh.h"
#include "TMOctreeAccelerator.h"
#include "AxisAlignedSlab.h"
#include "FlatContainer.h"
#include "BoundingVolumeHierarchy.h"

std::shared_ptr<TriangleMesh> AssetLoader::load( const std::string & filename )
{
    Assimp::Importer importer;
    const aiScene * scene = nullptr;
    
    scene = importer.ReadFile( filename,
                               aiProcess_Triangulate
                               | aiProcess_FindInvalidData
                               | aiProcess_GenSmoothNormals
                               //| aiProcess_GenNormals
                               | aiProcess_FixInfacingNormals
                               );
    
    if( !scene ) {
        fprintf( stderr, "Failed to load %s : %s\n", filename.c_str(), importer.GetErrorString() );
        return nullptr;
    }
    
    printf( "Loaded %s\n", filename.c_str() );
    printf( " - # meshes -> %u\n", scene->mNumMeshes );
    
    aiMesh ** meshes = scene->mMeshes;
    // FIXME - just getting the first mesh for now
    unsigned int mesh_index = 0;
    
    aiMesh * mesh = meshes[mesh_index];

    printf( "Mesh[%u] Has Positions=%d(%u) Faces=%d(%u) Normals=%d Bones=%d\n", mesh_index, 
            (int) mesh->HasPositions(), mesh->mNumVertices,
            (int) mesh->HasFaces(), mesh->mNumFaces,
            (int) mesh->HasNormals(),
            (int) mesh->HasBones() );
    
    auto trimesh = std::make_shared<TriangleMesh>();
    
    trimesh->vertices.resize( mesh->mNumVertices );
    trimesh->normals.resize( mesh->mNumVertices );
    trimesh->triangles.resize( mesh->mNumFaces );
 
    for( unsigned int vi = 0; vi < mesh->mNumVertices; ++vi ) {
        const auto v = mesh->mVertices[vi];
        auto n = mesh->mNormals[vi];

        // TEMP
        // FIXME[DAC]: This is a hacky fix, especially since normal vectors
        //             should never be zero length. The bug is probably in assimp.
        if( isnan(n.x) ) {
            n.x = n.y = n.z = 0.0f;
        }

        trimesh->vertices[vi].set( v.x, v.y, v.z );
        trimesh->normals[vi].set( n.x, n.y, n.z );
    }

    for( unsigned int ti = 0; ti < mesh->mNumFaces; ++ti ) {
        const auto t = mesh->mFaces[ti];
        trimesh->triangles[ti].vi[0] = t.mIndices[0];
        trimesh->triangles[ti].vi[1] = t.mIndices[1];
        trimesh->triangles[ti].vi[2] = t.mIndices[2];
    }

    // Shift to the canonical position and size
    // TODO[DAC]: Make this configurable
    trimesh->makeCanonical();

    printf("TriMesh bounds: ");
    auto bounds = trimesh->getAxisAlignedBounds();
    bounds->print();

    return trimesh;
}

std::shared_ptr<Container> AssetLoader::loadMultiPart( const std::string & filename )
{
    Assimp::Importer importer;
    const aiScene * scene = nullptr;
    
    // note: scene is destroyed automatically when importer is destroyed
    scene = importer.ReadFile( filename,
                               aiProcess_Triangulate
                               | aiProcess_FindInvalidData
                               | aiProcess_GenSmoothNormals
                               //| aiProcess_GenNormals
                               | aiProcess_FixInfacingNormals
                               );
    
    if( !scene ) {
        fprintf( stderr, "Failed to load %s\n", filename.c_str() );
        return nullptr;
    }
    
    printf( "Loaded %s\n", filename.c_str() );
    printf( " - # meshes -> %u\n", scene->mNumMeshes );

    auto container = std::make_shared<FlatContainer>();
    
    aiMesh ** meshes = scene->mMeshes;

    for( unsigned int mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index )
    {
        aiMesh * mesh = meshes[mesh_index];

        printf( "Mesh[%u] Has Positions=%d(%u) Faces=%d(%u) Normals=%d Bones=%d\n", mesh_index, 
                (int) mesh->HasPositions(), mesh->mNumVertices,
                (int) mesh->HasFaces(), mesh->mNumFaces,
                (int) mesh->HasNormals(),
                (int) mesh->HasBones() );

        auto trimesh = std::make_shared<TriangleMesh>();

        trimesh->vertices.resize( mesh->mNumVertices );
        trimesh->normals.resize( mesh->mNumVertices );
        trimesh->triangles.resize( mesh->mNumFaces );

        for( unsigned int vi = 0; vi < mesh->mNumVertices; ++vi ) {
            const auto v = mesh->mVertices[vi];
            auto n = mesh->mNormals[vi];

            // TEMP
            // FIXME[DAC]: This is a hacky fix, especially since normal vectors
            //             should never be zero length. The bug is probably in assimp.
            if( isnan(n.x) ) {
                n.x = n.y = n.z = 0.0f;
            }

            trimesh->vertices[vi].set( v.x, v.y, v.z );
            trimesh->normals[vi].set( n.x, n.y, n.z );
        }

        for( unsigned int ti = 0; ti < mesh->mNumFaces; ++ti ) {
            const auto t = mesh->mFaces[ti];
            trimesh->triangles[ti].vi[0] = t.mIndices[0];
            trimesh->triangles[ti].vi[1] = t.mIndices[1];
            trimesh->triangles[ti].vi[2] = t.mIndices[2];
        }

        TMOctreeAccelerator * trimesh_octree = new TMOctreeAccelerator( *trimesh );
        trimesh_octree->build();
        trimesh->accelerator = trimesh_octree;

#if 0
        // TEMP: return bounding box for fast rendering
        container->add( trimesh->getAxisAlignedBounds() );
#else
        container->add( trimesh );

        printf("TriMesh bounds: ");
        auto bounds = trimesh->getAxisAlignedBounds();
        bounds->print();
#endif
    }

#if 1
    // Build a bounding volume hierarchy. This may speed up rendering of models
    // with many parts
    // TODO[DAC]: Test how well this helps with various models to determine if there
    //            are cases where we might want to just return a flat container or
    //            merge parts into a single mesh like with loadMultiPartMerged()
    auto bvh = std::make_shared<BoundingVolumeHierarchy>();

    bvh->build( container );

    container = std::make_shared<FlatContainer>();
    container->add( bvh );
#endif

    // TODO[DAC]: Do a makeCanonical() resizing on the collection of meshes
    // TODO[DAC]: Put them in a smarter container, like a BV hierarchy

    return container;
}

// Loads a multipart mesh and merges the pieces into a single mesh object
std::shared_ptr<TriangleMesh> AssetLoader::loadMultiPartMerged( const std::string & filename )
{
    auto container = loadMultiPart( filename );

    if( !container )
        return nullptr;

    unsigned int num_vertices = 0;
    unsigned int num_normals = 0;
    unsigned int num_triangles = 0;

    // Find out how much we need to allocate
    for( int i = 0; i < container->size(); i++ ) {
        auto mesh = std::dynamic_pointer_cast<TriangleMesh>(container->at(i));
        num_vertices += mesh->vertices.size();
        num_normals += mesh->normals.size();
        num_triangles += mesh->triangles.size();
    }

    auto ubermesh = std::make_shared<TriangleMesh>();

    ubermesh->vertices.resize( num_vertices );
    ubermesh->normals.resize( num_normals );
    ubermesh->triangles.resize( num_triangles );

    printf("Ubermesh vertices=%u normals=%u triangles=%u\n", num_vertices, num_normals, num_triangles);

    // Copy contents of each mesh into our uber mesh
    //   Make sure to offset the triangle indices by the starting vertex index for each part
    unsigned int vertex_index = 0;
    unsigned int normal_index = 0;
    unsigned int triangle_index = 0;

    for( int i = 0; i < container->size(); i++ ) {
        auto mesh = std::dynamic_pointer_cast<TriangleMesh>(container->at(i));

        unsigned int part_vertex_start = vertex_index;

        for( unsigned int vi = 0; vi < mesh->vertices.size(); vi++, vertex_index++ ) {
            ubermesh->vertices[vertex_index] = mesh->vertices[vi];
        }
        for( unsigned int ni = 0; ni < mesh->normals.size(); ni++, normal_index++ ) {
            ubermesh->normals[normal_index] = mesh->normals[ni];
        }
        for( unsigned int ti = 0; ti < mesh->triangles.size(); ti++, triangle_index++ ) {
            ubermesh->triangles[triangle_index].vi[0] = mesh->triangles[ti].vi[0] + part_vertex_start;
            ubermesh->triangles[triangle_index].vi[1] = mesh->triangles[ti].vi[1] + part_vertex_start;
            ubermesh->triangles[triangle_index].vi[2] = mesh->triangles[ti].vi[2] + part_vertex_start;
        }
    }

    if( ubermesh->vertices.size() == 0 ) {
        return ubermesh;
    }

    // Shift to the canonical position and size
    // TODO: Should we have this here or make it the caller's responsibility?
    // TODO: Make this configurable
    //ubermesh->makeCanonical();

    printf("Ubermesh bounds: ");
    auto bounds = ubermesh->getAxisAlignedBounds();
    bounds->print();

    // TODO: Maybe take this out and make it the caller's responsibility to build accel structures
    TMOctreeAccelerator * octree = new TMOctreeAccelerator( *ubermesh );
    octree->build();
    ubermesh->accelerator = octree;

    return ubermesh;
}

