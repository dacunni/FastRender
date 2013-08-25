//
//  AssetLoader.cpp
//  FastRender
//
//  Created by David Cunningham on 8/15/13.
//
//

// Using the Open Asset Import Library ("assimp" - http://assimp.sourceforge.net/ )
#include <assimp/assimp.hpp>
#include <assimp/aiScene.h>
#include <assimp/aiPostProcess.h>

#include "AssetLoader.h"
#include "TriangleMesh.h"


Traceable * AssetLoader::load( const std::string & filename )
{
    Assimp::Importer importer;
    const aiScene * scene = nullptr;
    
    scene = importer.ReadFile( filename, aiProcess_Triangulate );
    
    if( !scene ) {
        printf( "Failed to load %s\n", filename.c_str() );
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
    trimesh->triangles.resize( mesh->mNumFaces );
 
    for( unsigned int vi = 0; vi < mesh->mNumVertices; ++vi ) {
        const auto v = mesh->mVertices[vi];
        trimesh->vertices[vi].set( v.x, v.y, v.z );

        // TEMP >>>
        const float scale = 10.0f;
        trimesh->vertices[vi][0] *= scale;
        trimesh->vertices[vi][1] *= scale;
        trimesh->vertices[vi][2] *= scale;
        trimesh->vertices[vi][0] += 0.0;
        trimesh->vertices[vi][1] += -1.0;
        trimesh->vertices[vi][2] += -5.0;
        //printf("V %f %f %f\n", v.x, v.y, v.z); // TEMP
        // TEMP <<<
    }

    for( unsigned int ti = 0; ti < mesh->mNumFaces; ++ti ) {
        const auto t = mesh->mFaces[ti];
        trimesh->triangles[ti].vi[0] = t.mIndices[0];
        trimesh->triangles[ti].vi[1] = t.mIndices[1];
        trimesh->triangles[ti].vi[2] = t.mIndices[2];
    }

    // IMPLEMENT ME
    
    return trimesh;
}