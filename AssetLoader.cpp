//
//  AssetLoader.cpp
//  FastRender
//
//  Created by David Cunningham on 8/15/13.
//
//

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

TriangleMesh * AssetLoader::load( const std::string & filename )
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
        fprintf( stderr, "Failed to load %s\n", filename.c_str() );
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
    
    TriangleMesh * trimesh = new TriangleMesh();
    
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

    //printf("TriMesh bounds: ");
    //std::unique_ptr<AxisAlignedSlab> bounds( trimesh->getAxisAlignedBounds() );
    //bounds->print();

    return trimesh;
}

Container * AssetLoader::loadMultiPart( const std::string & filename )
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
        fprintf( stderr, "Failed to load %s\n", filename.c_str() );
        return nullptr;
    }
    
    printf( "Loaded %s\n", filename.c_str() );
    printf( " - # meshes -> %u\n", scene->mNumMeshes );

    FlatContainer * container = new FlatContainer();
    
    aiMesh ** meshes = scene->mMeshes;

    for( unsigned int mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index )
    {
        aiMesh * mesh = meshes[mesh_index];

        printf( "Mesh[%u] Has Positions=%d(%u) Faces=%d(%u) Normals=%d Bones=%d\n", mesh_index, 
                (int) mesh->HasPositions(), mesh->mNumVertices,
                (int) mesh->HasFaces(), mesh->mNumFaces,
                (int) mesh->HasNormals(),
                (int) mesh->HasBones() );

        TriangleMesh * trimesh = new TriangleMesh();

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

        container->add( trimesh );
    }

    // TODO[DAC]: Do a makeCanonical() resizing on the collection of meshes
    // TODO[DAC]: Put the in a smarter container, like a BV hierarchy

    return container;
}
