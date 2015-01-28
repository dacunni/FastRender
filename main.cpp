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
#include "SimpleCamera.h"
#include "AmbientOcclusionShader.h"
#include "BasicDiffuseSpecularShader.h"
#include "FlatContainer.h"
#include "AxisAlignedSlab.h"
#include "TriangleMesh.h"
#include "Timer.h"
#include "BoundingVolume.h"
#include "TMOctreeAccelerator.h"
#include "Material.h"
#include "TestScenes.h"

RandomNumberGenerator rng;

Scene * buildScene()
{
    Scene * scene = new Scene();

    printf( "Building scene\n" );
    Timer build_scene_timer;
    build_scene_timer.start();
	FlatContainer * container = new FlatContainer();
	
    addLightingTest2( container );
    //addSlabGrid( container );
    //addOffsetCubes( container );
    //addLitBunny( container );
    //addGroundPlane( container );

	scene->root = container;
    scene->buildLightList();
    build_scene_timer.stop();
    printf( "Build scene: %f seconds\n", build_scene_timer.elapsed() );

    return scene;
}

void testScene()
{
    Sphere sphere( Vector4( 0.0, 0.0, 0.0 ), 5.0 );
	Ray ray;
	RayIntersection intersection;
    
    int imageSize = 320;
    int imageWidth = imageSize, imageHeight = imageSize;
    Artifacts artifacts( imageWidth, imageHeight );
    
    Matrix4x4 projectionMatrix;
    projectionMatrix.identity();
    
    Scene * scene = buildScene();
    
    // intersect with scene
    float xmin = -0.15, xmax = 0.15, ymin = -0.15, ymax = 0.15;
    SimpleCamera camera( rng, xmin, xmax, ymin, ymax, imageWidth, imageHeight );

    //Shader * shader = new AmbientOcclusionShader();
    Shader * shader = new BasicDiffuseSpecularShader();

    float anim_progress = 0.0f; // blend factor from 0.0 to 1.0 throughout animation
    int num_frames = 1;
    int num_rays_per_pixel = 50;
    for( int frame_index = 0; frame_index < num_frames; frame_index++ ) {
        if( num_frames > 1 )
            anim_progress = (float) frame_index / (num_frames - 1);
        printf("Frame %d (%.2f %%)\n", frame_index, anim_progress * 100.0);
        Vector4 rot_axis( 0.0, 1.0, 0.0 );
        rot_axis.normalize();
        float min_angle = -0.0, max_angle = 0.55;
        float angle = (anim_progress * (max_angle - min_angle)) + min_angle;
        Transform rotation = makeRotation( angle, rot_axis );
        Vector4 begin_xlate( 0.0, 0.0, 5.0 ), end_xlate( 2.0, 0.25, -2.0 );
        Vector4 xlate = interp( begin_xlate,end_xlate, anim_progress);
        Transform translation = makeTranslation( xlate );
        Transform xform;
        xform = compose( rotation, xform );
        xform = compose( translation, xform );

        RGBColor pixel_color( 0.0, 0.0, 0.0 );
        Vector4 pixel_normal;
        float pixel_distance;

        printf("Rendering scene:\n");
        Timer pixel_render_timer;
        for( int row = 0; row < imageHeight; row++ ) {
            if( row % (imageHeight / 10) == 0 )
                printf("ROW %d / %d\n", row, imageHeight);

            for( int col = 0; col < imageWidth; col++ ) {
                pixel_render_timer.start();
                pixel_color.setRGB( 0.0, 0.0, 0.0 );
                pixel_normal.set( 0.0, 0.0, 0.0 );
                pixel_distance = 0.0f;

                int num_hits = 0;
                for( int ri = 0; ri < num_rays_per_pixel; ri++ ) {
                    Vector4 d = camera.vectorThrough( row, col );
                    ray.direction = mult( xform.fwd, d );
                    ray.direction.normalize();
                    Vector4 o = Vector4( 0.0, 0.0, 0.0 );  // default camera at origin
                    ray.origin = mult( xform.fwd, o );

                    intersection = RayIntersection();
                    bool hit = scene->intersect( ray, intersection );

                    if( hit ) {
                        num_hits++;

                        if( num_hits == 1 ) { // first hit
                            pixel_normal = intersection.normal;
                            pixel_distance = intersection.distance;
                            //intersection.position.fprintCSV( artifacts.intersections_file );
                        }

                        shader->shade( *scene, rng, intersection );
                        pixel_color.accum( intersection.sample.color );
                    }
                } // ray index

                pixel_color.scale( 1.0f / num_rays_per_pixel );
                pixel_render_timer.stop();
                artifacts.setPixelTime( row, col, pixel_render_timer.elapsed() );
                artifacts.setPixelColorRGB( row, col, pixel_color.r, pixel_color.g, pixel_color.b );
                artifacts.setPixelNormal( row, col, pixel_normal );
                artifacts.setPixelDepth( row, col, pixel_distance );

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

