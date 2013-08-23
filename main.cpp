#include <iostream>
#include <fstream>
#include <stdio.h>

// TEMP >>>
#include <Magick++.h>
#include <math.h>
// TEMP <<<

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

RandomNumberGenerator rng;


void testScene() {
    Sphere sphere( Vector4( 0.0, 0.0, 0.0 ), 5.0 );
	Ray ray( Vector4( 0.0, 0.0, 3.0 ), Vector4( 0.0, 0.0, -1.0 ) );
	RayIntersection intersection;
    
    int imageSize = 200;
    int imageWidth = imageSize, imageHeight = imageSize;
    
    Matrix4x4 projectionMatrix;
    projectionMatrix.identity();
    
    printf( "Building scene\n" );
    Timer build_scene_timer;
    build_scene_timer.start();
	Scene scene;
	FlatContainer * container = new FlatContainer();
	
    const int numSpheres = 0;
    
	for( int si = 0; si < numSpheres; si++ ) {
#if 0
        float x = rng.uniformRange( -1.5, 1.5 );
        float y = rng.uniformRange( -1.5, 1.5 );
        float z = rng.uniformRange( -10.0, -3.0 );
        container->add( new AxisAlignedSlab( x - 0.1, y - 0.1, z - 0.1,
                                             x + 0.1, y + 0.1, z + 0.1 ) );
#else
		container->add( new Sphere( Vector4( rng.uniformRange( -1.5, 1.5 ),
                                             rng.uniformRange( -1.5, 1.5 ),
                                             rng.uniformRange( -10.0, -3.0 ) ),
 								    0.15 ) );
#endif
	}
    
    //container->add( new Sphere( Vector4( 0.0, 0.0, -6.5 ), 0.3 ) );

#if 0
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
#endif

#if 0
    // Offset cubes for testing AO
    container->add( new AxisAlignedSlab( -0.1, -0.1-0.6, -1.0,
                                          0.1,  0.1-0.6, -1.2 ) );
    container->add( new AxisAlignedSlab( -0.1, -0.0-0.6, -1.2,
                                          0.1,  0.3-0.6, -1.4 ) );
#endif
    
#if 1
    container->add( new AxisAlignedSlab( -5.0, 0.4-0.9, -0.5,
                                          5.0, -0.0-0.9, -10.0 ) );
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
    // Low res dragon
    Traceable * mesh = loader.load( "/Users/dacunni/Projects/FastRender/models/stanford/dragon/dragon_vrip_res4.ply" );
    //Traceable * mesh = loader.load( "/Users/dacunni/Projects/FastRender/models/stanford/dragon/dragon_vrip_res3.ply" );
    // Low res bunnies
    //Traceable * mesh = loader.load( "/Users/dacunni/Projects/FastRender/models/stanford/bunny/reconstruction/bun_zipper_res4.ply" );
    //Traceable * mesh = loader.load( "/Users/dacunni/Projects/FastRender/models/stanford/bunny/reconstruction/bun_zipper_res3.ply" );
    //Traceable * mesh = loader.load( "/Users/dacunni/Projects/FastRender/models/stanford/bunny/reconstruction/bun_zipper_res2.ply" );
    // Full res bunny
    //Traceable * mesh = loader.load( "/Users/dacunni/Projects/FastRender/models/stanford/bunny/reconstruction/bun_zipper.ply" );
    //container->add( mesh );
    
    BoundingVolume * meshBB = new BoundingVolume();
    meshBB->buildAxisAligned( mesh );
    container->add( meshBB );
    
	scene.root = container;
    build_scene_timer.stop();
    printf( "Build scene: %f seconds\n", build_scene_timer.elapsed() );
    
    Magick::Image image( Magick::Geometry( imageWidth, imageHeight ), "black" );
    image.magick( "png" ); // set the output file type
    Magick::Image normal_image( Magick::Geometry( imageWidth, imageHeight ), "black" );
    normal_image.magick( "png" ); // set the output file type
    Magick::Image depth_image( Magick::Geometry( imageWidth, imageHeight ), Magick::ColorRGB(0.0f, 0.0f, 0.0f) );
    depth_image.magick( "png" ); // set the output file type

    // FIXME - better handling of output files
    //FILE * intersections_file = fopen( "/Users/dacunni/Projects/FastRender/output/intersections.txt", "w" );
    // write eye location
    //ray.origin.fprintCSV(intersections_file);
    // intersect with scene
    float xmin = -0.15;
    float xmax = 0.15;
    float ymin = -0.15;
    float ymax = 0.15;
    
    for( int row = 0; row < imageHeight; row++ ) {
        printf("ROW %d / %d\n", row, imageHeight); // TEMP
        for( int col = 0; col < imageWidth; col++ ) {
            ray.direction[0] = (float) col / imageWidth * (xmax - xmin) + xmin;
            ray.direction[1] = (float) (imageHeight - row - 1) / imageHeight * (ymax - ymin) + ymin;
            ray.direction[2] = -1.0;
            ray.direction.normalize();
            intersection = RayIntersection();
            bool hit = scene.intersect( ray, intersection );
            if( hit ) {
                //intersection.position.fprintCSV( intersections_file );
#if 1
                
                normal_image.pixelColor(col, row,
                                        Magick::ColorRGB(intersection.normal.x * 0.5 + 0.5,
                                                         intersection.normal.y * 0.5 + 0.5,
                                                         intersection.normal.z * 0.5 + 0.5));
                //float output_depth = 1.0f / logf(intersection.distance + M_E);
                float output_depth = (1.0 - (intersection.distance - 3.0) / 10.0);
                output_depth = std::min( std::max( output_depth, 0.0f ), 1.0f );
                
                depth_image.pixelColor(col, row, Magick::ColorRGB(output_depth, output_depth, output_depth));

#if 1
                // playing with ambient occlusion
                Vector4 to_source( ray.direction );
                to_source.negate();
                //normal_image.pixelColor(col, row,
                //                        Magick::ColorRGB(to_source.x * 0.5 + 0.5,
                //                                         to_source.y * 0.5 + 0.5,
                //                                         to_source.z * 0.5 + 0.5));

                const unsigned int num_ao_rays = 10;
                unsigned int hits = 0;
                float value = 0;
                Ray ao_ray;
                RayIntersection ao_intersection;
                //ao_ray.origin = intersection.position;
                Vector4 offset( 0.0, 0.0, 0.0 );
                scale( intersection.normal, 0.1, offset );
                add( intersection.position, offset, ao_ray.origin );
                
                for( unsigned int aori = 0; aori < num_ao_rays; aori++ ) {
                    rng.uniformSurfaceUnitHalfSphere( intersection.normal, ao_ray.direction );
                    
                    ao_intersection = RayIntersection();
                    ao_intersection.min_distance = 0.1;
                    //if( scene.intersect( ao_ray, ao_intersection ) ) {
                    if( scene.intersectsAny( ao_ray, intersection.min_distance ) ) {
                        hits++;
                    }
                }
                value = 1.0f - (float) hits / num_ao_rays;
                image.pixelColor(col, row, Magick::ColorRGB( value, value, value ));
                //normal_image.pixelColor(col, row,
                //                        Magick::ColorRGB(ao_ray.direction.x * 0.5 + 0.5,
                //                                         ao_ray.direction.y * 0.5 + 0.5,
                //                                         ao_ray.direction.z * 0.5 + 0.5));

#else
                image.pixelColor(col, row, Magick::Color("white"));
#endif

#endif
            }
        }
    }
    //fclose( intersections_file );
	
    Timer image_flush_timer;
    image_flush_timer.start();
    image.write( "/Users/dacunni/Projects/FastRender/output/framebuffer.png" );
    normal_image.write( "/Users/dacunni/Projects/FastRender/output/normals.png" );
    depth_image.write( "/Users/dacunni/Projects/FastRender/output/depth.png" );
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

