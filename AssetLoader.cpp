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
#include "Logger.h"
#include "TriangleMesh.h"
#include "TMOctreeAccelerator.h"
#include "AxisAlignedSlab.h"
#include "FlatContainer.h"
#include "BoundingVolumeHierarchy.h"

AssetLoader::AssetLoader()
    : logger(getLogger())
{

}

const aiScene * loadAssimpScene( Logger & logger, Assimp::Importer & importer, const std::string filename )
    throw(AssetFileNotFoundException)
{
    const aiScene * scene = nullptr;
    
    // NOTE: Scene is destroyed automatically when importer is destroyed!
    logger.normalf("Assimp loading %s", filename.c_str());
    scene = importer.ReadFile( filename,
                               aiProcess_Triangulate
                               | aiProcess_FindInvalidData
                               | aiProcess_GenSmoothNormals
                               //| aiProcess_GenNormals
                               | aiProcess_FixInfacingNormals
                               );
    importer.ApplyPostProcessing( aiProcess_CalcTangentSpace );

    if( !scene ) {
        logger.fatalf("Failed to load %s", filename.c_str());
        throw AssetFileNotFoundException();
    }
    
    logger.debugf("Loaded '%s' - # meshes -> %u", filename.c_str(), scene->mNumMeshes);

    for( unsigned int mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index ) {
        aiMesh * mesh = scene->mMeshes[mesh_index];
        bool has_uv = mesh->GetNumUVChannels() > 0 && mesh->mNumUVComponents[0] >= 2;
        logger.debugf("Mesh[%u] Has Positions=%d(%u) Faces=%d(%u) Normals=%d Tangents=%d UV=%d Bones=%d", mesh_index, 
                     (int) mesh->HasPositions(), mesh->mNumVertices,
                     (int) mesh->HasFaces(), mesh->mNumFaces,
                     (int) mesh->HasNormals(),
                     (int) mesh->HasTangentsAndBitangents(),
                     (int) has_uv,
                     (int) mesh->HasBones());
    }
    return scene;
}

// Helper for loadTriangleArray
static void const constructTriangleMesh( const aiMesh * mesh,
                                         TriangleMesh & trimesh,
                                         const Transform & transform )
{
    bool has_uv = mesh->GetNumUVChannels() > 0 && mesh->mNumUVComponents[0] >= 2;

    trimesh.vertices.resize( mesh->mNumVertices );
    trimesh.normals.resize( mesh->mNumVertices );
    trimesh.triangles.resize( mesh->mNumFaces );
    if( has_uv ) {
        trimesh.textureUVCoords.resize( mesh->mNumVertices );
    }

    for( unsigned int vi = 0; vi < mesh->mNumVertices; ++vi ) {
        const auto v = mesh->mVertices[vi];
        auto n = mesh->mNormals[vi];

        // TEMP
        // FIXME[DAC]: This is a hacky fix, especially since normal vectors
        //             should never be zero length. The bug is probably in assimp.
        if( isnan(n.x) ) {
            n.x = n.y = n.z = 0.0f;
        }

#if 1
        trimesh.vertices[vi] = mult( transform.fwd, Vector4( v.x, v.y, v.z, 1.0f ) );
        trimesh.normals[vi] = mult( transform.fwd, Vector4( n.x, n.y, n.z, 0.0f ) );
#else
        trimesh.vertices[vi].set( v.x, v.y, v.z );
        trimesh.normals[vi].set( n.x, n.y, n.z );
#endif
        if( has_uv ) {
            const auto tc = mesh->mTextureCoords[0][vi];
            trimesh.textureUVCoords[vi] = { tc.x, tc.y };
        }
    }

    for( unsigned int ti = 0; ti < mesh->mNumFaces; ++ti ) {
        const auto t = mesh->mFaces[ti];
        trimesh.triangles[ti].vi[0] = t.mIndices[0];
        trimesh.triangles[ti].vi[1] = t.mIndices[1];
        trimesh.triangles[ti].vi[2] = t.mIndices[2];
    }
}

void copy( Matrix4x4 & m, const aiMatrix4x4 & aim )
{
    m = Matrix4x4( aim.a1, aim.a2, aim.a3, aim.a4,
                   aim.b1, aim.b2, aim.b3, aim.b4,
                   aim.c1, aim.c2, aim.c3, aim.c4,
                   aim.d1, aim.d2, aim.d3, aim.d4 );
}

// Helper for loadTriangleArray
static void appendToTriangleArray( const aiScene * scene,
                                   const aiNode * node,
                                   TriangleMeshArray & array,
                                   const Transform & parentTransform,
                                   int depth = 0 )
{
    aiMatrix4x4 aifwd = node->mTransformation;
    aiMatrix4x4 airev = aifwd.Inverse();

    for(int i = 0; i < depth; i++) printf(" ");
    printf("node nm %20s #msh %3u #ch %3u", node->mName.C_Str(),
           node->mNumMeshes, node->mNumChildren);
    printf(" xf");
    printf(" %4.1f %4.1f %4.1f %4.1f", aifwd.a1, aifwd.a2, aifwd.a3, aifwd.a4);
    printf(";%4.1f %4.1f %4.1f %4.1f", aifwd.b1, aifwd.b2, aifwd.b3, aifwd.b4);
    printf(";%4.1f %4.1f %4.1f %4.1f", aifwd.c1, aifwd.c2, aifwd.c3, aifwd.c4);
    printf(";%4.1f %4.1f %4.1f %4.1f", aifwd.d1, aifwd.d2, aifwd.d3, aifwd.d4);
    printf(" det %3.2f\n", aifwd.Determinant());

    Matrix4x4 fwd, rev;
    copy(fwd, aifwd);
    copy(rev, airev);

    Transform localTransform(fwd, rev);
    Transform transform = compose(parentTransform, localTransform);

    //transform.fwd.print(); // TEMP

    for( int mi = 0; mi < node->mNumMeshes; mi++ ) {
        auto trimesh = std::make_shared<TriangleMesh>();
        constructTriangleMesh( scene->mMeshes[node->mMeshes[mi]], *trimesh, transform );
        array.push_back( trimesh );
    }

    for( int ci = 0; ci < node->mNumChildren; ci++ ) {
        appendToTriangleArray( scene, node->mChildren[ci], array, transform, depth+1 );
    }
}

void AssetLoader::loadTriangleArray( const std::string & filename,
                                     TriangleMeshArray & array ) throw(AssetFileNotFoundException)
{
    logger.debug("Loading triangle mesh array\n");
    Assimp::Importer importer;
    const aiScene * scene = loadAssimpScene( logger, importer, filename );
    Transform baseTransform;
    appendToTriangleArray( scene, scene->mRootNode, array, baseTransform );
}

std::shared_ptr<TriangleMesh> AssetLoader::load( const std::string & filename,
                                                 bool build_accelerator ) throw(AssetFileNotFoundException)
{
    Assimp::Importer importer;
    const aiScene * scene = loadAssimpScene( logger, importer, filename );
    aiMesh ** meshes = scene->mMeshes;
    // FIXME - just getting the first mesh for now
    unsigned int mesh_index = 0;
    
    aiMesh * mesh = meshes[mesh_index];
    bool has_uv = mesh->GetNumUVChannels() > 0 && mesh->mNumUVComponents[0] >= 2;
    auto trimesh = std::make_shared<TriangleMesh>();
    
    trimesh->vertices.resize( mesh->mNumVertices );
    trimesh->normals.resize( mesh->mNumVertices );
    trimesh->triangles.resize( mesh->mNumFaces );
    if( has_uv ) {
        trimesh->textureUVCoords.resize( mesh->mNumVertices );
    }
 
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
        if( has_uv ) {
            const auto tc = mesh->mTextureCoords[0][vi];
            trimesh->textureUVCoords[vi] = { tc.x, tc.y };
        }
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

    auto bounds = trimesh->getAxisAlignedBounds();
    logger.normal() << "TriMesh bounds: " << bounds->toString();

    if( build_accelerator ) {
        TMOctreeAccelerator * trimesh_octree = new TMOctreeAccelerator( *trimesh );
        trimesh_octree->build();
        trimesh->accelerator = trimesh_octree;
    }

    return trimesh;
}

std::shared_ptr<Container> AssetLoader::loadMultiPart( const std::string & filename,
                                                       bool build_accelerator ) throw(AssetFileNotFoundException)
{
    TriangleMeshArray array;
    loadTriangleArray( filename, array );

    auto container = std::make_shared<FlatContainer>();
    
    for( auto trimesh : array ) {
        if( build_accelerator ) {
            TMOctreeAccelerator * trimesh_octree = new TMOctreeAccelerator( *trimesh );
            trimesh_octree->build();
            trimesh->accelerator = trimesh_octree;
        }

        container->add( trimesh );

        printf("TriMesh bounds: ");
        auto bounds = trimesh->getAxisAlignedBounds();
        bounds->print();
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
std::shared_ptr<TriangleMesh> AssetLoader::loadMultiPartMerged( const std::string & filename,
                                                                bool build_accelerator ) throw(AssetFileNotFoundException)
{
    TriangleMeshArray array;
    loadTriangleArray( filename, array );

    unsigned int num_vertices = 0;
    unsigned int num_normals = 0;
    unsigned int num_triangles = 0;

    // Find out how much we need to allocate
    for( auto mesh : array ) {
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

    for( auto mesh : array ) {
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
    //       - Thought: Just have makeCananical() generate the appropriate transform and make it
    //                  the user's responsibility
    // TODO: Make this configurable
    //ubermesh->makeCanonical();

    printf("Ubermesh bounds: ");
    auto bounds = ubermesh->getAxisAlignedBounds();
    bounds->print();

    if( build_accelerator ) {
        TMOctreeAccelerator * octree = new TMOctreeAccelerator( *ubermesh );
        octree->build();
        ubermesh->accelerator = octree;
    }

    return ubermesh;
}

