#include <iostream>
#include <fstream>
#include <stdio.h>

// TEMP >>>
//#include <Magick++.h>
#include <math.h>
// TEMP <<<

#include "Artifacts.h"
#include "Matrix.h"
#include "Transform.h"
#include "Ray.h"
#include "Sphere.h"
#include "RandomNumberGenerator.h"
#include "Scene.h"
#include "Shader.h"
#include "AmbientOcclusionShader.h"
#include "BasicDiffuseSpecularShader.h"
#include "FlatContainer.h"
#include "AxisAlignedSlab.h"
#include "TriangleMesh.h"
#include "Timer.h"
#include "AssetLoader.h"
#include "BoundingVolume.h"
#include "TMOctreeAccelerator.h"
#include "Material.h"
#include "TestScenes.h"

RandomNumberGenerator rng;

void testScene()
{
    Sphere sphere( Vector4( 0.0, 0.0, 0.0 ), 5.0 );
	Ray ray;
	RayIntersection intersection;
    
    int imageSize = 256;
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
    
    //container->add( new Sphere( Vector4( 0.0, 0.0, -5.5 ), 0.5 ) );
    //container->add( new Sphere( Vector4( 1.0, 0.0, -5.0 ), 0.5 ) );
    //container->add( new Sphere( Vector4( -1.0, 0.0, -3.0 ), 0.5 ) );

    //container->add( new AxisAlignedSlab( 0.5,  0.5, -10.0,
    //                                     1.0, -0.5,  5.0 ) );
    
    //addSlabGrid( container );
    addOffsetCubes( container );
    
    // Cube emitter for some light
    //AxisAlignedSlab * emitter = new AxisAlignedSlab( -2.0, +0.25, +1.0,
    //                                                 -0.9, +0.5, -4.0 );
    //AxisAlignedSlab * emitter = new AxisAlignedSlab( +0.3, +1.0, +1.0,
    //                                                 +2.0, +1.5, -4.0 );
    //AxisAlignedSlab * emitter = new AxisAlignedSlab( +0.3, +10.00, +5.0,
    //                                                 +2.0, +1.5, +6.0 );
    Sphere * emitter = new Sphere( Vector4( 1.0, 0.8, -3.0 ), 0.25 );
    emitter->material = new Material();
    float power = 100.0f;
    emitter->material->emittance.setRGB( power, power, power );
    container->add( emitter );

#if 1
    // makeshift ground plane
    container->add( new AxisAlignedSlab( -5.0, -0.5, +10.0,
                                          5.0, -0.9, -10.0 ) );
    //container->add( new AxisAlignedSlab( -10.0, -5.0, +10.0,
    //                                     -1.5,  5.0, -10.0 ) );
    
#endif
    
#if 0
    // DEBUG ME
    TriangleMesh * ground = new TriangleMesh();
    makeTriangleMeshGroundPlatform( *ground, 200.0 );
    container->add( ground );
#endif
    
    AssetLoader loader;
    std::string modelPath = "models";

    // dragon
    std::string dragonPath = modelPath + "/stanford/dragon/reconstruction";
    //TriangleMesh * mesh = loader.load( dragonPath + "/dragon_vrip_res4.ply" );
    //TriangleMesh * mesh = loader.load( dragonPath + "/dragon_vrip_res3.ply" );
    //TriangleMesh * mesh = loader.load( dragonPath + "/dragon_vrip_res2.ply" );
    TriangleMesh * mesh = loader.load( dragonPath + "/dragon_vrip.ply" );
    
    // bunnies
    std::string bunnyPath = modelPath + "/stanford/bunny/reconstruction";
    //TriangleMesh * mesh = loader.load( bunnyPath + "/bun_zipper_res4.ply" );
    //TriangleMesh * mesh = loader.load( bunnyPath + "/bun_zipper_res3.ply" );
    //TriangleMesh * mesh = loader.load( bunnyPath + "/bun_zipper_res2.ply" );
    //TriangleMesh * mesh = loader.load( bunnyPath + "/bun_zipper.ply" );

    //TriangleMesh * mesh = loader.load( modelPath + "/stanford/lucy.ply" );
    //TriangleMesh * mesh = loader.load( modelPath + "/stanford/Armadillo.ply" );

    if( !mesh ) {
        fprintf( stderr, "Error loading mesh\n" );
        return;
    }

    mesh->material = new DiffuseMaterial( 1.0f, 0.0f, 1.0f );

    printf("Building octree\n");
    TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
    mesh_octree->build();
    mesh->accelerator = mesh_octree;
    BoundingVolume * meshBB = new BoundingVolume();
    meshBB->buildAxisAligned( mesh );
    container->add( meshBB );
    
	scene.root = container;
    build_scene_timer.stop();
    printf( "Build scene: %f seconds\n", build_scene_timer.elapsed() );
    
    // intersect with scene
    float xmin = -0.15, xmax = 0.15, ymin = -0.15, ymax = 0.15;

    Shader * shader = new AmbientOcclusionShader();
    //Shader * shader = new BasicDiffuseSpecularShader();

    float anim_progress = 0.0f; // blend factor from 0.0 to 1.0 throughout animation
    int num_frames = 1;
    int num_rays_per_pixel = 1;
    for( int frame_index = 0; frame_index < num_frames; frame_index++ ) {
        if( num_frames > 1 )
            anim_progress = (float) frame_index / (num_frames - 1);
        printf("Frame %d (%.2f %%)\n", frame_index, anim_progress * 100.0);
        Vector4 rot_axis( 0.0, 1.0, 0.0 );
        //Vector4 rot_axis( 0.0, 0.0, 1.0 );
        rot_axis.normalize();
        //float angle = ((float) frame_index / num_frames * 2.0 - 1.0) * 0.1;
        //float angle = 0.0, min_angle = 0.0, max_angle = M_PI / 2.0;
        //float angle = 0.0, min_angle = -M_PI / 4.0, max_angle = M_PI / 4.0;
        float angle = 0.0, min_angle = -0.0, max_angle = 0.55;
        angle = (anim_progress * (max_angle - min_angle)) + min_angle;
        Transform rotation = makeRotation( angle, rot_axis );
        //Vector4 begin_xlate( 0.0, 0.0, 5.0 ), end_xlate( 0.0, 0.0, 5.0 );
        //Vector4 begin_xlate( 0.0, -0.25, 0.0 ), end_xlate( 0.0, 0.25, 0.0 );
        Vector4 begin_xlate( 0.0, 0.0, 5.0 ), end_xlate( 2.0, 0.25, -2.0 );
        Vector4 xlate;
        interp( begin_xlate, end_xlate, anim_progress, xlate);
        Transform translation = makeTranslation( xlate );
        //Transform scaling = makeScaling( 0.15, 0.15, 0.15 );
        Transform xform;
        xform = compose( rotation, xform );
        xform = compose( translation, xform );

        RGBColor pixel_color( 0.0, 0.0, 0.0 );
        printf("Rendering scene:\n");
        Timer pixel_render_timer;
        for( int row = 0; row < imageHeight; row++ ) {
            if( row % (imageHeight / 10) == 0 )
                printf("ROW %d / %d\n", row, imageHeight); // TEMP

            for( int col = 0; col < imageWidth; col++ ) {
                pixel_render_timer.start();
                pixel_color.setRGB( 0.0, 0.0, 0.0 );
                for( int ri = 0; ri < num_rays_per_pixel; ri++ ) {
                    float x_jitter = rng.uniformRange( -0.5 * (xmax - xmin) / (float) imageWidth,
                                                       0.5 * (xmax - xmin) / (float) imageWidth );
                    float y_jitter = rng.uniformRange( -0.5 * (ymax - ymin) / (float) imageHeight,
                                                       0.5 * (ymax - ymin) / (float) imageHeight );
                    ray.direction[0] = (float) col / imageWidth * (xmax - xmin) + xmin;
                    ray.direction[0] += x_jitter;
                    ray.direction[1] = (float) (imageHeight - row - 1) / imageHeight * (ymax - ymin) + ymin;
                    ray.direction[1] += y_jitter;
                    ray.direction[2] = -1.0f;
                    ray.direction[3] = 0.0f;
                    ray.direction.normalize();
                    Vector4 d = ray.direction;
                    mult( xform.fwd, d, ray.direction );
                    ray.direction.normalize();
                    Vector4 o = Vector4( 0.0, 0.0, 0.0 );  // default camera at origin
                    mult( xform.fwd, o, ray.origin );

                    intersection = RayIntersection();
                    bool hit = scene.intersect( ray, intersection );

                    if( hit ) {
                        if( ri == 0 ) { // FIXME - first hit, not this
                            artifacts.setPixelNormal( row, col, intersection.normal );
                            artifacts.setPixelDepth( row, col, intersection.distance );
                            //intersection.position.fprintCSV( artifacts.intersections_file );
                        }

                        shader->shade( scene, rng, intersection );
                        pixel_color.r += intersection.sample.color.r;
                        pixel_color.g += intersection.sample.color.g;
                        pixel_color.b += intersection.sample.color.b;
                    }
                } // ray index

                pixel_color.r /= (float) num_rays_per_pixel;
                pixel_color.g /= (float) num_rays_per_pixel;
                pixel_color.b /= (float) num_rays_per_pixel;
                pixel_render_timer.stop();
                artifacts.setPixelTime( row, col, pixel_render_timer.elapsed() );
                artifacts.setPixelColorRGB( row, col, pixel_color.r, pixel_color.g, pixel_color.b );

            } // col
        } // row
        pixel_render_timer.stop();
        if( frame_index < num_frames - 1 )
            artifacts.startNewFrame();
    } // frame_index
	
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

int main (int argc, char * const argv[]) 
{
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

