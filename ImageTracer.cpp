/*
 *  ImageTracer.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 1/22/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "ImageTracer.h"

// for intersection test logging
#include "AxisAlignedSlab.h"
#include "Sphere.h"
#include "TriangleMesh.h"


ImageTracer::ImageTracer( unsigned int w, unsigned int h,
                          unsigned int nframes,
                          unsigned int rayspp )
    : rng(),
      image_width( w ),
      image_height( h ),
      camera( rng, -0.15, 0.15, -0.15, 0.15, w, h ),
      artifacts( w, h ),
      rays_per_pixel( rayspp ),
      num_frames( nframes )
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
            if( row % (image_height / 10) == 0 )
                printf("ROW %d / %d\n", row, image_height);
            for( unsigned int col = 0; col < image_width; ++col ) {
                beginRenderPixel( row, col );
                for( unsigned int ray_index = 0; ray_index < rays_per_pixel; ++ray_index ) {
                    tracePixelRay( row, col, ray_index );
                }
                endRenderPixel( row, col );
            }
        }
        endFrame( frame );
    }

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

void ImageTracer::beginFrame( unsigned int frame_index )
{
    if( num_frames > 1 )
        anim_progress = (float) frame_index / (num_frames - 1);
    else
        anim_progress = 0.0f;

    if( camera_transform_cb ) {
        camera.transform = camera_transform_cb( anim_progress );
    }
    //camera.transform.print();
}

void ImageTracer::endFrame( unsigned int frame_index )
{
    if( frame_index < num_frames - 1 )
        artifacts.startNewFrame();
}

void ImageTracer::beginRenderPixel( unsigned int row, unsigned int col )
{
    pixel_render_timer.start();
    pixel_color.setRGB( 0.0, 0.0, 0.0 );
    pixel_normal.set( 0.0, 0.0, 0.0 );
    pixel_distance = 0.0f;
    num_hits = 0;
}

void ImageTracer::endRenderPixel( unsigned int row, unsigned int col )
{
    pixel_color.scale( 1.0f / rays_per_pixel );
    pixel_render_timer.stop();
    artifacts.setPixelTime( row, col, pixel_render_timer.elapsed() );
    artifacts.setPixelColorRGB( row, col, pixel_color.r, pixel_color.g, pixel_color.b );
    artifacts.setPixelNormal( row, col, pixel_normal );
    artifacts.setPixelDepth( row, col, pixel_distance );
}

void ImageTracer::tracePixelRay( unsigned int row, unsigned int col,
                                 unsigned int ray_index )
{
    //printf("tracePixelRay( row: %u, col: %u, ray_index: %u )\n");
    Ray ray = camera.rayThrough( row, col );
    RayIntersection intersection = RayIntersection();
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
}


