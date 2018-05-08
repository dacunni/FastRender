/*
 *  TestScenes.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 2/22/14
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include <sys/stat.h>

#include "Container.h"
#include "AxisAlignedSlab.h"
#include "RandomNumberGenerator.h"
#include "Sphere.h"
#include "Material.h"
#include "AssetLoader.h"
#include "TMOctreeAccelerator.h"
#include "BoundingVolume.h"
#include "EnvironmentMap.h"
#include "FlatContainer.h"
#include "BasicDiffuseSpecularShader.h"
#include "TestScenes.h"
#include "Logger.h"

////////////////////////////////////////////////////////////////////////////
//
// Test Registry
//
std::vector<TestRegistryEntryErased *> testRegistry;

// Prints a list of all tests in the test registry
void printTests() {
    std::cout << "Tests (" << testRegistry.size() << ")" << std::endl;
    unsigned int index = 0;
    for( auto & test : testRegistry ) {
        std::cout << "\t" << index << "  " << test->name << std::endl;
        index++;
    }
}

void runTest( unsigned int index )
{
    auto & test = testRegistry[index];
    std::cout << "---[ Running Test : " << test->name  << " ]---" << std::endl;
    test->run();
}

// Calls the run() method on all tests in the registry
void runTests() {
    for( auto & test : testRegistry ) {
        std::cout << "---[ Running Test : " << test->name  << " ]---" << std::endl;
        test->run();
    }
}
////////////////////////////////////////////////////////////////////////////
//
// TestScene Class
//
TestScene::TestScene( const std::string & output_path, const std::string & test_name )
  : scene(new Scene()),
    container(new FlatContainer()),
    name(test_name),
    output_dir(output_path),
    tracer(nullptr),
    rays_per_pixel(10),
    // TODO[DAC]: Figure out an easy way for tests to override these settings
    image_width(256),
    image_height(256),
    anim_frames(1),
    logger(getLogger())
{
    logger.normal() << "TestScene name: " << name;
}

TestScene::~TestScene()
{

}

void TestScene::setup()
{
    logger.normalf("Creating image tracer %d x %d, %d frames, %d rays per pixel",
                   image_width, image_height, anim_frames, rays_per_pixel);
    logger.normalf("Output directory: %s", output_dir.c_str());

    tracer = new ImageTracer( image_width, image_height,
                              anim_frames, rays_per_pixel );

#if 0
    // Create a subdirectory in the output directory named with the test name, and put standard
    // output files in there
    tracer->artifacts.output_path = output_dir + "/" + name;
    mkdir(tracer->artifacts.output_path.c_str(), 0777);
    tracer->artifacts.file_prefix = "";
#else
    // Prefix each output file with the test name and put them in the output directory
    tracer->artifacts.output_path = output_dir;
    tracer->artifacts.file_prefix = name + "_";
#endif

    tracer->shader = new BasicDiffuseSpecularShader();

    tracer->setCameraTransform( compose(
        // move up a bit
        makeTranslation( 0.0, 0.5, 0.0 ),
        // rotate so we are looking down
        makeRotation( -0.2, Vector4(1, 0, 0) ),
        // back away from the origin
        makeTranslation( 0.0, 0.0, 10.0 )
        ) );
}

void TestScene::buildScene()
{
    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                    +10.0, -1.0, -10.0 );
    container->add( floor );

#if 0
    float size = 1.0;
    float half_size = size / 2.0;
    auto cube = new AxisAlignedSlab( -half_size, 0.0, -half_size,
                                       half_size, size, half_size );
    cube->material = std::make_shared<DiffuseMaterial>( 0.5, 0.5, 1.0 );
    container->add( cube );
#else
    auto sphere = std::make_shared<Sphere>( 0.0, 1.0, 0, 1.0 );
    sphere->material = std::make_shared<DiffuseMaterial>( 0.5, 0.5, 1.0 );
    container->add( sphere );
#endif

    scene->env_map = std::make_shared<ArcLightEnvironmentMap>();

    tracer->shader = new BasicDiffuseSpecularShader();
}

void TestScene::render()
{
    tracer->scene = scene;
    tracer->scene->buildLightList();
    tracer->render();
}

////////////////////////////////////////////////////////////////////////////

void addSlabGrid( std::shared_ptr<Container> container )
{
    container->add( std::make_shared<AxisAlignedSlab>( -0.1-0.45, -0.1, -1.0,
                                                       0.1-0.45,  0.1, -5.3 ) );
    container->add( std::make_shared<AxisAlignedSlab>( -0.1+0.45, -0.1, -1.0,
                                                       0.1+0.45,  0.1, -5.3 ) );
    container->add( std::make_shared<AxisAlignedSlab>( -0.1, -0.1+0.45, -1.0,
                                                       0.1,  0.1+0.45, -5.3 ) );
    container->add( std::make_shared<AxisAlignedSlab>( -0.1, -0.1-0.45, -1.0,
                                                       0.1,  0.1-0.45, -5.3 ) );

    container->add( std::make_shared<AxisAlignedSlab>( -0.1-0.45, -0.1-0.45, -1.0,
                                                       0.1-0.45,  0.1-0.45, -5.3 ) );
    container->add( std::make_shared<AxisAlignedSlab>( -0.1-0.45, -0.1+0.45, -1.0,
                                                       0.1-0.45,  0.1+0.45, -5.3 ) );
    container->add( std::make_shared<AxisAlignedSlab>( -0.1+0.45, -0.1-0.45, -1.0,
                                                       0.1+0.45,  0.1-0.45, -5.3 ) );
    container->add( std::make_shared<AxisAlignedSlab>( -0.1+0.45, -0.1+0.45, -1.0,
                                                       0.1+0.45,  0.1+0.45, -5.3 ) );
}

void addRandomSpheres( std::shared_ptr<Container> container, RandomNumberGenerator & rng, int numSpheres )
{
	for( int si = 0; si < numSpheres; si++ ) {
        container->add( std::make_shared<Sphere>( Vector4( rng.uniformRange( -1.5, 1.5 ),
                                                           rng.uniformRange( -1.5, 1.5 ),
                                                           rng.uniformRange( -10.0, -3.0 ) ),
                                                  0.15 ) );
    }
}

void addRandomCubes( std::shared_ptr<Container> container, RandomNumberGenerator & rng, int numCubes )
{
	for( int si = 0; si < numCubes; si++ ) {
        float x = rng.uniformRange( -1.5, 1.5 );
        float y = rng.uniformRange( -1.5, 1.5 );
        float z = rng.uniformRange( -10.0, -3.0 );
        container->add( std::make_shared<AxisAlignedSlab>( x - 0.1, y - 0.1, z - 0.1,
                                                           x + 0.1, y + 0.1, z + 0.1 ) );
	}
}

void addOffsetCubes( std::shared_ptr<Container> container )
{
    // Offset cubes for testing AO
    auto cube1 = std::make_shared<AxisAlignedSlab>(  0.1, -0.5, -1.0,
                                                     0.3, -0.3, -1.2 );
    auto cube2 = std::make_shared<AxisAlignedSlab>( -0.1, -0.5, -1.2,
                                                     0.1, -0.3, -1.4 );
    cube1->material = std::make_shared<DiffuseMaterial>( 1.0, 0.0, 0.0 );
    container->add( cube1 );
    container->add( cube2 );
}

void addBunny( std::shared_ptr<Container> container )
{
    AssetLoader loader;
    std::string modelPath = "models";

    // bunnies
    std::string bunnyPath = modelPath + "/stanford/bunny/reconstruction";
    auto mesh = loader.load( bunnyPath + "/bun_zipper_res2.ply" );
    //auto mesh = loader.load( bunnyPath + "/bun_zipper_res4.ply" );

    if( !mesh ) {
        fprintf( stderr, "Error loading mesh\n" );
        return;
    }

    //mesh->material = std::make_shared<DiffuseMaterial>( 0.0f, 0.66, 0.42f ); // emerald green
    mesh->material = std::make_shared<MirrorMaterial>();

    printf("Building octree\n");
    auto mesh_octree = new TMOctreeAccelerator( *std::dynamic_pointer_cast<TriangleMesh>(mesh) );
    mesh_octree->build();
    mesh->accelerator = mesh_octree;
    mesh->transform = std::make_shared<Transform>();
    *mesh->transform = makeTranslation( Vector4( 0.0, 0.2, -0.5 ) );
    container->add( mesh );
}

void addLitBunny( std::shared_ptr<Container> container ) {
    addBunny( container );
    addSphereLight( container,
                    Vector4( 2.0, 3.8, -4.0 ), 0.5,
                    RGBColor( 1.0, 1.0, 1.0 ), 15.0 );
}

void addSphereLight( std::shared_ptr<Container> container,
                     const Vector4 & center, float r,
                     const RGBColor & color,
                     float power )
{
    auto emitter = std::make_shared<Sphere>( center, r );
    emitter->material = std::make_shared<Material>();
    emitter->material->emittance = color;
    emitter->material->emittance.scale( power );
    container->add( emitter );
}

void addGroundPlane( std::shared_ptr<Container> container )
{
    // makeshift ground plane
    auto floor = std::make_shared<AxisAlignedSlab>( -5.0, -0.5, +10.0,
                                                     5.0, -0.9, -10.0 );
    floor->material = std::make_shared<DiffuseMaterial>( 1.0, 1.0, 1.0 );
    container->add( floor );
    
#if 0
    // DEBUG ME
    TriangleMesh * ground = new TriangleMesh();
    makeTriangleMeshGroundPlatform( *ground, 200.0 );
    container->add( ground );
#endif
}

void addTransformedCubes( std::shared_ptr<Container> container )
{
    // Offset cubes for testing AO
    float cube_size = 0.2;
    auto cube1 = std::make_shared<AxisAlignedSlab>( 0.0, 0.0, 0.0,
                                                   cube_size );
    auto cube2 = std::make_shared<AxisAlignedSlab>( 0.0, 0.0, 0.0,
                                                    cube_size );
    auto cube3 = std::make_shared<AxisAlignedSlab>( 0.0, 0.0, 0.0,
                                                    cube_size );

    cube1->material = std::make_shared<DiffuseMaterial>( 1.0, 0.0, 0.0 );
    cube2->material = std::make_shared<DiffuseMaterial>( 0.0, 0.0, 1.0 );
    cube3->material = std::make_shared<DiffuseMaterial>( 0.0, 1.0, 0.0 );

    cube1->transform = std::make_shared<Transform>();
    *cube1->transform = makeTranslation( Vector4( 0.1, 0.5, -1.0 ) );

    cube2->transform = std::make_shared<Transform>();
    *cube2->transform = 
        compose(
            makeTranslation( Vector4( 0.0, 0.15, -1.0 ) ),
            compose( 
                makeRotation( M_PI / 4.0, Vector4( 0.0, 1.0, 0.0 ) ),
                makeRotation( M_PI / 4.0, Vector4( 0.0, 0.0, 1.0 ) )
                )
            );

    cube3->transform = std::make_shared<Transform>();
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

void addLightingTest1( std::shared_ptr<Container> container )
{
    addGroundPlane( container );
    addSphereLight( container,
                    Vector4( 1.0, 0.8, -3.0 ), 0.5,
                    RGBColor( 1.0, 1.0, 1.0 ), 20.0 );
    auto cube1 = std::make_shared<AxisAlignedSlab>(  0.1, -0.5, -1.0,
                                                     0.3, -0.3, -1.2 );
    auto cube2 = std::make_shared<AxisAlignedSlab>( -0.1, -0.5, -1.2,
                                                     0.1, -0.3, -1.4 );
    container->add( cube1 );
    container->add( cube2 );
    auto cube3 = std::make_shared<AxisAlignedSlab>( -1.0, -0.5, -6.0,
                                                     1.0 );
    container->add( cube3 );
}

// cube pyramid
void addLightingTest2( std::shared_ptr<Container> container )
{
    addGroundPlane( container );
    addSphereLight( container,
                    Vector4( 1.0, 4.0, -2.0 ), 1.5,
                    RGBColor( 1.0, 1.0, 1.0 ), 20.0 );

    float size = 0.4;
    Vector4 c( -0.6, -0.5, -2.0 ); // back corner

    container->add( std::make_shared<AxisAlignedSlab>( c.x,        c.y,        c.z,        size ) );
    container->add( std::make_shared<AxisAlignedSlab>( c.x + size, c.y,        c.z,        size ) );
    container->add( std::make_shared<AxisAlignedSlab>( c.x       , c.y + size, c.z,        size ) );
    container->add( std::make_shared<AxisAlignedSlab>( c.x       , c.y       , c.z + size, size ) );

}

// cube pyramid
void addLightingTest3( std::shared_ptr<Container> container )
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
                    container->add( std::make_shared<AxisAlignedSlab>( c.x + (float) xs * size,
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


void addLightingTest4( std::shared_ptr<Container> container )
{
    float zoff = -5.0;

    container->add( std::make_shared<Sphere>( 0.0, 0.0, zoff - 2.0, 0.2 ) );
    container->add( std::make_shared<Sphere>( -0.4, 0.0, zoff, 0.2 ) );
    container->add( std::make_shared<Sphere>( 0.4, 0.0, zoff, 0.2 ) );
    container->add( std::make_shared<Sphere>( 0.0, -0.4, zoff, 0.2 ) );
    container->add( std::make_shared<Sphere>( 0.0, 0.4, zoff, 0.2 ) );

    float lightoff = zoff + 0.3;

    addSphereLight( container,
                    Vector4( -0.4, 0.4, lightoff ), 0.1,
                    RGBColor( 0.0, 0.0, 1.0 ), 15.0 );
    addSphereLight( container,
                    Vector4( 0.4, 0.4, lightoff ), 0.1,
                    RGBColor( 0.0, 1.0, 0.0 ), 15.0 );
    addSphereLight( container,
                    Vector4( 0.4, -0.4, lightoff ), 0.1,
                    RGBColor( 1.0, 0.0, 0.0 ), 15.0 );
    addSphereLight( container,
                    Vector4( -0.4, -0.4, lightoff ), 0.1,
                    RGBColor( 1.0, 1.0, 0.0 ), 15.0 );

}

//////////////////////////////////////////
//     Mirror Test
//////////////////////////////////////////
void addMirrors( std::shared_ptr<Container> container )
{
    auto cube1 = std::make_shared<AxisAlignedSlab>( -5.0, -5.0, -5.0,
                                                     5.0, 5.0, -5.2 );
    cube1->material = std::make_shared<MirrorMaterial>();
    container->add( cube1 );
    auto cube2 = std::make_shared<AxisAlignedSlab>( -2.5, -5.0, -5.0,
                                                    -2.7, 5.0, 5.0 );
    cube2->material = std::make_shared<MirrorMaterial>();
    container->add( cube2 );
}


//////////////////////////////////////////
//     Standard Material Test Models
//////////////////////////////////////////
std::shared_ptr<TriangleMesh> loadMaterialTestModel( AssetLoader & loader )
{
    std::string modelBasePath = "models";
    //auto mesh = loader.load( modelBasePath + "/dacunni/material_test1.obj" );
    //auto mesh = loader.load( modelBasePath + "/dacunni/material_test1.stl" );
    auto mesh = loader.load( modelBasePath + "/tf3dm.com/soccerball/untitled.ply" );
    //auto mesh = loader.load( modelBasePath + "/uvsphere.ply" );
    //auto mesh = loader.loadMultiPartMerged( modelBasePath + "/test_objects/mori/testObj.obj" );
#if 0
    auto mesh = loader.loadMultiPartMerged( modelBasePath + "/test_objects/mitsuba/mitsuba.obj" );
#endif
#if 0
    mesh->makeCanonical();
#endif
#if 1
    mesh->accelerator = new TMOctreeAccelerator( *mesh );
    mesh->accelerator->build();
#endif
    return mesh;
}

std::shared_ptr<HDRImageEnvironmentMap> loadStPetersEnvironmentMap()
{
    return std::make_shared<HDRImageEnvironmentMap>("light_probes/debevec/stpeters_probe.float", 1500, 1500);
}
std::shared_ptr<HDRImageEnvironmentMap> loadUffiziEnvironmentMap()
{
    return std::make_shared<HDRImageEnvironmentMap>("light_probes/debevec/uffizi_probe.float", 1500, 1500);
}
std::shared_ptr<HDRImageEnvironmentMap> loadRNLEnvironmentMap()
{
    return std::make_shared<HDRImageEnvironmentMap>("light_probes/debevec/rnl_probe.float", 900, 900);
}
std::shared_ptr<HDRImageEnvironmentMap> loadGraceEnvironmentMap()
{
    return std::make_shared<HDRImageEnvironmentMap>("light_probes/debevec/grace_probe.float", 1000, 1000);
}

