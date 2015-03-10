/*
 *  TestScenes.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 2/22/14
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "Container.h"
#include "AxisAlignedSlab.h"
#include "RandomNumberGenerator.h"
#include "Sphere.h"
#include "Material.h"
#include "AssetLoader.h"
#include "TMOctreeAccelerator.h"
#include "BoundingVolume.h"
#include "TestScenes.h"

void addSlabGrid( Container * container )
{
    container->add( new AxisAlignedSlab( -0.1-0.45, -0.1, -1.0,
                                         0.1-0.45,  0.1, -5.3 ) );
    container->add( new AxisAlignedSlab( -0.1+0.45, -0.1, -1.0,
                                         0.1+0.45,  0.1, -5.3 ) );
    container->add( new AxisAlignedSlab( -0.1, -0.1+0.45, -1.0,
                                         0.1,  0.1+0.45, -5.3 ) );
    container->add( new AxisAlignedSlab( -0.1, -0.1-0.45, -1.0,
                                         0.1,  0.1-0.45, -5.3 ) );

    container->add( new AxisAlignedSlab( -0.1-0.45, -0.1-0.45, -1.0,
                                         0.1-0.45,  0.1-0.45, -5.3 ) );
    container->add( new AxisAlignedSlab( -0.1-0.45, -0.1+0.45, -1.0,
                                         0.1-0.45,  0.1+0.45, -5.3 ) );
    container->add( new AxisAlignedSlab( -0.1+0.45, -0.1-0.45, -1.0,
                                         0.1+0.45,  0.1-0.45, -5.3 ) );
    container->add( new AxisAlignedSlab( -0.1+0.45, -0.1+0.45, -1.0,
                                         0.1+0.45,  0.1+0.45, -5.3 ) );
}

void addRandomSpheres( Container * container, RandomNumberGenerator & rng, int numSpheres )
{
	for( int si = 0; si < numSpheres; si++ ) {
		container->add( new Sphere( Vector4( rng.uniformRange( -1.5, 1.5 ),
                                            rng.uniformRange( -1.5, 1.5 ),
                                            rng.uniformRange( -10.0, -3.0 ) ),
                                   0.15 ) );
	}
}

void addRandomCubes( Container * container, RandomNumberGenerator & rng, int numCubes )
{
	for( int si = 0; si < numCubes; si++ ) {
        float x = rng.uniformRange( -1.5, 1.5 );
        float y = rng.uniformRange( -1.5, 1.5 );
        float z = rng.uniformRange( -10.0, -3.0 );
        container->add( new AxisAlignedSlab( x - 0.1, y - 0.1, z - 0.1,
                                             x + 0.1, y + 0.1, z + 0.1 ) );
	}
}

void addOffsetCubes( Container * container )
{
    // Offset cubes for testing AO
    AxisAlignedSlab * cube1 = new AxisAlignedSlab(  0.1, -0.5, -1.0,
                                                    0.3, -0.3, -1.2 );
    AxisAlignedSlab * cube2 = new AxisAlignedSlab( -0.1, -0.5, -1.2,
                                                    0.1, -0.3, -1.4 );
    cube1->material = new DiffuseMaterial( 1.0, 0.0, 0.0 );
    container->add( cube1 );
    container->add( cube2 );
}

void addLitBunny( Container * container )
{
    AssetLoader loader;
    std::string modelPath = "models";

    // bunnies
    std::string bunnyPath = modelPath + "/stanford/bunny/reconstruction";
    TriangleMesh * mesh = loader.load( bunnyPath + "/bun_zipper_res3.ply" );

    if( !mesh ) {
        fprintf( stderr, "Error loading mesh\n" );
        return;
    }

    mesh->material = new DiffuseMaterial( 0.0f, 0.66, 0.42f );

    printf("Building octree\n");
    TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
    mesh_octree->build();
    mesh->accelerator = mesh_octree;

    BoundingVolume * meshBB = new BoundingVolume();
    meshBB->buildAxisAligned( mesh );
    container->add( meshBB );
    meshBB->transform = new Transform();
    *meshBB->transform = makeTranslation( Vector4( 0.0, 0.2, 0.0 ) );

    // Add a light
    addSphereLight( container,
                    Vector4( -2.0, 0.8, -4.0 ), 0.5,
                    RGBColor( 1.0, 1.0, 1.0 ), 15.0 );
}

void addSphereLight( Container * container,
                     const Vector4 & center, float r,
                     const RGBColor & color,
                     float power )
{
    Sphere * emitter = new Sphere( center, r );
    emitter->material = new Material();
    emitter->material->emittance = color;
    emitter->material->emittance.scale( power );
    container->add( emitter );
}

void addGroundPlane( Container * container )
{
    // makeshift ground plane
    AxisAlignedSlab * floor = new AxisAlignedSlab( -5.0, -0.5, +10.0,
                                                   5.0, -0.9, -10.0 );
    floor->material = new DiffuseMaterial( 1.0, 1.0, 1.0 );
    container->add( floor );
    
#if 0
    // DEBUG ME
    TriangleMesh * ground = new TriangleMesh();
    makeTriangleMeshGroundPlatform( *ground, 200.0 );
    container->add( ground );
#endif
}

void addTransformedCubes( Container * container )
{
    // Offset cubes for testing AO
    float cube_size = 0.2;
    AxisAlignedSlab * cube1 = new AxisAlignedSlab( 0.0, 0.0, 0.0,
                                                   cube_size );
    AxisAlignedSlab * cube2 = new AxisAlignedSlab( 0.0, 0.0, 0.0,
                                                   cube_size );
    AxisAlignedSlab * cube3 = new AxisAlignedSlab( 0.0, 0.0, 0.0,
                                                   cube_size );

    cube1->material = new DiffuseMaterial( 1.0, 0.0, 0.0 );
    cube2->material = new DiffuseMaterial( 0.0, 0.0, 1.0 );
    cube3->material = new DiffuseMaterial( 0.0, 1.0, 0.0 );

    cube1->transform = new Transform();
    *cube1->transform = makeTranslation( Vector4( 0.1, 0.5, -1.0 ) );

    cube2->transform = new Transform();
    *cube2->transform = 
        compose(
            makeTranslation( Vector4( 0.0, 0.15, -1.0 ) ),
            compose( 
                makeRotation( M_PI / 4.0, Vector4( 0.0, 1.0, 0.0 ) ),
                makeRotation( M_PI / 4.0, Vector4( 0.0, 0.0, 1.0 ) )
                )
            );

    cube3->transform = new Transform();
    *cube3->transform = 
        compose(
            makeTranslation( Vector4( -0.3, 0.0, -1.0 ) ),
            makeScaling( 0.2, 3.0, 2.0 )
            );

    container->add( cube1 );
    container->add( cube2 );
    container->add( cube3 );

    addSphereLight( container,
                    Vector4( 3.0, 3.0, -1.0 ), 0.5,
                    RGBColor( 1.0, 1.0, 1.0 ), 200.0 );

    addGroundPlane( container );
}

//////////////////////////////////////////
// Lighting Tests
//////////////////////////////////////////

void addLightingTest1( Container * container )
{
    addGroundPlane( container );
    addSphereLight( container,
                    Vector4( 1.0, 0.8, -3.0 ), 0.5,
                    RGBColor( 1.0, 1.0, 1.0 ), 20.0 );
    AxisAlignedSlab * cube1 = new AxisAlignedSlab(  0.1, -0.5, -1.0,
                                                    0.3, -0.3, -1.2 );
    AxisAlignedSlab * cube2 = new AxisAlignedSlab( -0.1, -0.5, -1.2,
                                                    0.1, -0.3, -1.4 );
    container->add( cube1 );
    container->add( cube2 );
    AxisAlignedSlab * cube3 = new AxisAlignedSlab( -1.0, -0.5, -6.0,
                                                    1.0 );
    container->add( cube3 );
}

// cube pyramid
void addLightingTest2( Container * container )
{
    addGroundPlane( container );
    addSphereLight( container,
                    Vector4( 1.0, 4.0, -2.0 ), 1.5,
                    RGBColor( 1.0, 1.0, 1.0 ), 20.0 );

    float size = 0.4;
    Vector4 c( -0.6, -0.5, -2.0 ); // back corner

    container->add( new AxisAlignedSlab( c.x,        c.y,        c.z,        size ) );
    container->add( new AxisAlignedSlab( c.x + size, c.y,        c.z,        size ) );
    container->add( new AxisAlignedSlab( c.x       , c.y + size, c.z,        size ) );
    container->add( new AxisAlignedSlab( c.x       , c.y       , c.z + size, size ) );

}

// cube pyramid
void addLightingTest3( Container * container )
{
    addGroundPlane( container );
    addSphereLight( container,
                    Vector4( 1.0, 0.8, -0.0 ), 0.5,
                    RGBColor( 1.0, 1.0, 1.0 ), 20.0 );

    float size = 0.4;
    Vector4 c( -0.6, -0.5, -2.0 ); // back corner

    // Create a partial pyramid of size pyramid_size blocks in each direction
    //    x
    //    x x
    //    x x x
    int pyramid_size = 5;

    for( int xs = 0; xs <= pyramid_size; xs++ ) {
        for( int ys = 0; ys <= pyramid_size; ys++ ) {
            for( int zs = 0; zs <= pyramid_size; zs++ ) {
                if( xs + ys + zs == pyramid_size ) {
                    container->add( new AxisAlignedSlab( c.x + (float) xs * size,
                                                         c.y + (float) ys * size,
                                                         c.z + (float) zs * size,
                                                         size ) );
                }
            }
        }
    }

    //container->add( new AxisAlignedSlab( c.x, c.y, c.z + 2.0 * size, size ) );

    //container->add( new AxisAlignedSlab( c.x + size, c.y, c.z, size ) );
    //container->add( new AxisAlignedSlab( c.x, c.y + size, c.z, size ) );
    //container->add( new AxisAlignedSlab( c.x, c.y, c.z + size, size ) );

}



