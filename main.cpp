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
#include "EnvironmentMap.h"

RandomNumberGenerator rng;

Scene * buildScene()
{
    Scene * scene = new Scene();

    printf( "Building scene\n" );
    Timer build_scene_timer;
    build_scene_timer.start();
	FlatContainer * container = new FlatContainer();
	
    //addLightingTest2( container );
    //addSlabGrid( container );
    addOffsetCubes( container );
    //addLitBunny( container );
    addBunny( container );
    addGroundPlane( container );
    //addTransformedCubes( container );
    //addLightingTest4( container );

    //Sphere * sphere = new Sphere( Vector4( 0.0, 0.0, 0.0 ), 0.1 );
    //container->add( sphere );

	scene->root = container;
    //scene->env_map = new TestPatternEnvironmentMap();
    scene->env_map = new ArcLightEnvironmentMap();
    scene->buildLightList();
    build_scene_timer.stop();
    printf( "Build scene: %f seconds\n", build_scene_timer.elapsed() );

    return scene;
}

class ImageTracer
{
    ImageTracer( unsigned int w, unsigned int h );
    virtual ~ImageTracer();

    virtual void render();
    virtual void beginFrame( unsigned int frame_index );
    virtual void endFrame( unsigned int frame_index );
    virtual void beginRenderPixel( unsigned int row, unsigned int col );
    virtual void endRenderPixel( unsigned int row, unsigned int col );
    virtual void tracePixelRay( unsigned int row, unsigned int col,
                                unsigned int ray_index );

    RandomNumberGenerator rng;
    SimpleCamera camera;
    Artifacts artifacts;
    Scene * scene;
    unsigned int image_width;
    unsigned int image_height;
    unsigned int num_frames;
    unsigned int rays_per_pixel;
};

ImageTracer::ImageTracer( unsigned int w, unsigned int h )
    : rng(),
      camera( rng, -0.15, 0.15, -0.15, 0.15, w, h ),
      artifacts( w, h ),
      rays_per_pixel( 100 ),
      num_frames( 1 )
{

}

ImageTracer::~ImageTracer()
{

}

void ImageTracer::render()
{
    for( unsigned int frame = 0; frame < num_frames; ++frame ) {
        beginFrame( frame );
        for( unsigned int row = 0; row < image_height; ++row ) {
            for( unsigned int col = 0; col < image_height; ++col ) {
                beginRenderPixel( row, col );
                for( unsigned int ray_index = 0; ray_index < rays_per_pixel; ++rays_per_pixel ) {
                    tracePixelRay( row, col, ray_index );
                }
                endRenderPixel( row, col );
            }
        }
        endFrame( frame );
    }
}

void ImageTracer::beginFrame( unsigned int frame_index )
{

}

void ImageTracer::endFrame( unsigned int frame_index )
{

}

void ImageTracer::beginRenderPixel( unsigned int row, unsigned int col )
{

}

void ImageTracer::endRenderPixel( unsigned int row, unsigned int col )
{

}

void ImageTracer::tracePixelRay( unsigned int row, unsigned int col,
                                 unsigned int ray_index )
{

}

void testScene()
{
    Sphere sphere( Vector4( 0.0, 0.0, 0.0 ), 5.0 );
	Ray ray;
	RayIntersection intersection;
    
    //int imageSize = 50;
    int imageSize = 320;
    //int imageSize = 512;
    //int imageSize = 1024;
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
    //int num_frames = 20;
    int num_rays_per_pixel = 100;
    printf("Rays per pixel: %d\n", num_rays_per_pixel);
    for( int frame_index = 0; frame_index < num_frames; frame_index++ ) {
        if( num_frames > 1 )
            anim_progress = (float) frame_index / (num_frames - 1);
        printf("Frame %d (%.2f %%)\n", frame_index, anim_progress * 100.0);
        Transform xform;
#if 0
        Transform rotation;
        Transform translation = makeTranslation( 0.0, 0.0, 5.0 );
#elif 1
        Vector4 rot_axis( -1.0, 1.0, 0.0 );
        rot_axis.normalize();
        float angle = 0.5;
        Transform rotation = makeRotation( angle, rot_axis );
        Transform translation = makeTranslation( 1.0, 2.0, 15.0 );
#else
        Vector4 rot_axis( 0.0, 1.0, 0.0 );
        rot_axis.normalize();
        float min_angle = -0.0, max_angle = 0.55;
        float angle = (anim_progress * (max_angle - min_angle)) + min_angle;
        Transform rotation = makeRotation( angle, rot_axis );
        Vector4 begin_xlate( 1.5, 1.5, 5.0 ), end_xlate( 2.0, 0.25, -2.0 );
        Vector4 xlate = interp( begin_xlate, end_xlate, anim_progress);
        Transform translation = makeTranslation( xlate );
#endif
        xform = compose( translation, xform );
        xform = compose( rotation, xform );

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
                    camera.transform = xform;
                    ray = camera.rayThrough( row, col );
                    intersection = RayIntersection();
                    bool hit = scene->intersect( ray, intersection );

                    if( hit ) {
                        num_hits++;

                        if( num_hits == 1 ) { // first hit
                            pixel_normal = intersection.normal;
                            pixel_distance = intersection.distance;
                            //intersection.position.fprintCSV( artifacts.intersections_file );
                        }

                        if( intersection.distance != FLT_MAX ) {
                            shader->shade( *scene, rng, intersection );
                        }
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

