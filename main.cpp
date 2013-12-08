#include <iostream>
#include <fstream>
#include <stdio.h>

// TEMP >>>
//#include <Magick++.h>
#include <math.h>
// TEMP <<<

#include "Artifacts.h"
#include "Matrix.h"
#include "Ray.h"
#include "Sphere.h"
#include "RandomNumberGenerator.h"
#include "Scene.h"
#include "FlatContainer.h"
#include "AxisAlignedSlab.h"
#include "TriangleMesh.h"
#include "Timer.h"
#include "AssetLoader.h"
#include "BoundingVolume.h"
#include "TMOctreeAccelerator.h"

RandomNumberGenerator rng;

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

void addRandomSpheres( Container * container, int numSpheres )
{
	for( int si = 0; si < numSpheres; si++ ) {
		container->add( new Sphere( Vector4( rng.uniformRange( -1.5, 1.5 ),
                                            rng.uniformRange( -1.5, 1.5 ),
                                            rng.uniformRange( -10.0, -3.0 ) ),
                                   0.15 ) );
	}
}

void addRandomCubes( Container * container, int numCubes )
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
    container->add( new AxisAlignedSlab(  0.1, -0.5, -1.0,
                                          0.3, -0.3, -1.2 ) );
    container->add( new AxisAlignedSlab( -0.1, -0.5, -1.2,
                                          0.1, -0.3, -1.4 ) );
}

// Ambient occlusion shader. Could probably use lots of improvement,
// especially by acceleration structures for meshes.
float shadeAmbientOcclusion( Scene & scene, RayIntersection & intersection ) 
{
    const unsigned int num_ao_rays = 8;
    unsigned int hits = 0;
    float value = 0;
    Ray ao_ray;
    RayIntersection ao_intersection;
    //ao_ray.origin = intersection.position;
    Vector4 offset( 0.0, 0.0, 0.0 );
    scale( intersection.normal, 0.01, offset ); // NOTE - Seems to help 
    add( intersection.position, offset, ao_ray.origin );

    for( unsigned int aori = 0; aori < num_ao_rays; aori++ ) {
        rng.uniformSurfaceUnitHalfSphere( intersection.normal, ao_ray.direction );

        ao_intersection = RayIntersection();
        ao_intersection.min_distance = 0.01;
        if( scene.intersectsAny( ao_ray, ao_intersection.min_distance ) ) {
            hits++;
        }
    }
    value = 1.0f - (float) hits / (float) num_ao_rays;

    return value;
}

void testScene()
{
    Sphere sphere( Vector4( 0.0, 0.0, 0.0 ), 5.0 );
	Ray ray( Vector4( 0.0, 0.0, 3.0 ), Vector4( 0.0, 0.0, -1.0 ) );
	RayIntersection intersection;
    
    int imageSize = 128;
    int imageWidth = imageSize, imageHeight = imageSize;
    Artifacts artifacts( imageWidth, imageHeight );
    
    Matrix4x4 projectionMatrix;
    projectionMatrix.identity();
    
    printf( "Building scene\n" );
    Timer build_scene_timer;
    build_scene_timer.start();
	Scene scene;
	FlatContainer * container = new FlatContainer();
	
    //addRandomSpheres( container, 30 );
    //addRandomCubes( container, 30 );
    
    container->add( new Sphere( Vector4( 0.0, 0.0, -5.5 ), 0.5 ) );
    container->add( new Sphere( Vector4( 1.0, 0.0, -5.0 ), 0.5 ) );
    container->add( new Sphere( Vector4( -1.0, 0.0, -3.0 ), 0.5 ) );

    //container->add( new AxisAlignedSlab( 0.5,  0.5, -10.0,
    //                                     1.0, -0.5,  5.0 ) );
    
    //addSlabGrid( container );
    addOffsetCubes( container );
    
#if 1
    container->add( new AxisAlignedSlab( -5.0, -0.5, +0.5,
                                          5.0, -0.9, -10.0 ) );
    //container->add( new AxisAlignedSlab(  0.7,  0.75-0.9, -4.2,
    //                                      0.9,  0.0-0.9, -4.4 ) );
    
#endif
    
#if 0
    TriangleMesh * tetra = new TriangleMesh();
    makeTriangleMeshTetrahedron( *tetra );
    container->add( tetra );
#endif
    
#if 0
    // DEBUG ME
    TriangleMesh * ground = new TriangleMesh();
    makeTriangleMeshGroundPlatform( *ground, 200.0 );
    container->add( ground );
#endif
    
    
    AssetLoader loader;
    std::string modelPath = "models";

    // Low res dragon
    //std::string dragonPath = modelPath + "stanford/dragon";
    //Traceable * mesh = loader.load( dragonPath + "/dragon_vrip_res4.ply" );
    //Traceable * mesh = loader.load( dragonPath + "/dragon_vrip_res3.ply" );
    
    // Low res bunnies
    std::string bunnyPath = modelPath + "/stanford/bunny/reconstruction";
    //Traceable * mesh = loader.load( bunnyPath + "/bun_zipper_res4.ply" );
    //Traceable * mesh = loader.load( bunnyPath + "/bun_zipper_res3.ply" );
    Traceable * mesh = loader.load( bunnyPath + "/bun_zipper_res2.ply" );
    //Traceable * mesh = loader.load( bunnyPath + "/bun_zipper.ply" );

    if( !mesh ) {
        fprintf( stderr, "Error loading mesh\n" );
        return;
    }

#if 1
    // TEMP >>> - working on octree
    TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
    mesh_octree->build();
    mesh_octree->print();
    printf("EARLY ABORT\n"); exit(0); // TEMP
    // TEMP <<<
#endif
    
    BoundingVolume * meshBB = new BoundingVolume();
    meshBB->buildAxisAligned( mesh );
    container->add( meshBB );
    
	scene.root = container;
    build_scene_timer.stop();
    printf( "Build scene: %f seconds\n", build_scene_timer.elapsed() );
    
    // intersect with scene
    float xmin = -0.15, xmax = 0.15, ymin = -0.15, ymax = 0.15;
    
    Timer pixel_render_timer;
    pixel_render_timer.start();
    double last_pixel_elapsed = 0.0;
    for( int row = 0; row < imageHeight; row++ ) {
        printf("ROW %d / %d\n", row, imageHeight); // TEMP
        for( int col = 0; col < imageWidth; col++ ) {
            ray.direction[0] = (float) col / imageWidth * (xmax - xmin) + xmin;
            ray.direction[1] = (float) (imageHeight - row - 1) / imageHeight * (ymax - ymin) + ymin;
            ray.direction[2] = -1.0;
            ray.direction.normalize();
            intersection = RayIntersection();
            //printf("Calling scene intersect\n"); // TEMP
            bool hit = scene.intersect( ray, intersection );
            if( hit ) {
#if 1
                artifacts.setPixelNormal( row, col, intersection.normal );
                artifacts.setPixelDepth( row, col, intersection.distance );
                //intersection.position.fprintCSV( artifacts.intersections_file );
                
#if 1
                float value = shadeAmbientOcclusion( scene, intersection );
                artifacts.setPixelColorMono( row, col, value );
#else
                artifacts.setPixelColorMono( row, col, 1.0f );
#endif

#endif
            }
            double pixel_elapsed = pixel_render_timer.elapsed();
            artifacts.setPixelTime( row, col, pixel_elapsed - last_pixel_elapsed );
            last_pixel_elapsed = pixel_elapsed;
        }
    }
    pixel_render_timer.stop();
	
    Timer image_flush_timer;
    image_flush_timer.start();
    artifacts.flush();
    image_flush_timer.stop();
    printf( "Image write: %f seconds\n", image_flush_timer.elapsed() );
    
    printf( "Intersection tests: AASlab: %lu Sphere: %lu TriangleMesh: %lu\n",
           AxisAlignedSlab::intersection_test_count,
           Sphere::intersection_test_count,
           TriangleMesh::intersection_test_count
           );
}

// TODO - make tests for Transforms

int main (int argc, char * const argv[]) {
    printf("FastRender\n");
    fflush(stdout);
    Timer total_run_timer;
    total_run_timer.start();

    rng.seedCurrentTime();

    testScene();
    
    total_run_timer.stop();
    printf("Done - Run time = %f seconds\n", total_run_timer.elapsed());
    fflush(stdout);
    return 0;
}

