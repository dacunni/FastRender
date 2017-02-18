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
    Timer image_flush_timer;

    // Pixel order randomization
    std::vector<unsigned int> randomized_indices;

    if( traversal_order == Randomized ) {
        unsigned int num_pixels = image_width * image_height;
        // Initialize with default order
        for( unsigned int i = 0; i < num_pixels; i++ ) {
            randomized_indices.push_back(i);
        }
        // Randomly swap each pixel with another
        for( unsigned int i = 0; i < num_pixels; i++ ) {
            unsigned int j = rand() % num_pixels;
            std::swap(randomized_indices[i], randomized_indices[j]);
        }
    }

    printf("ImageTracer: Tracing scene\n"
           "  image_width = %u  image_height = %u\n"
           "  num_frames = %u  rays_per_pixel = %u\n",
           image_width, image_height,
           num_frames, rays_per_pixel);

    image_flush_timer.start();
    for( unsigned int frame = 0; frame < num_frames; ++frame ) {
        printf("FRAME %4d / %4d\n", frame + 1, num_frames);
        beginFrame( frame );
        if( traversal_nesting == SamplePosition ) {
            for( unsigned int sample_index = 0; sample_index < rays_per_pixel; sample_index++ ) {
                for( unsigned int row = 0; row < image_height; ++row ) {
                    if( image_flush_timer.elapsed() > min_flush_period_seconds ) {
                        printf("Flushing artifacts (progress: frame = %.2f %% anim = %.2f %%)\n",
                               (float) (image_height * sample_index + row) / (image_height * rays_per_pixel) * 100.0f,
                               num_frames > 1 ? (float) frame / (num_frames - 1) * 100.0f : 0.0f);
                        artifacts.flush();
                        image_flush_timer.start(); // reset timer
                    }
                    for( unsigned int col = 0; col < image_width; ++col ) {
                        if( traversal_order == Randomized ) {
                            unsigned orig_index = row * image_width + col;
                            unsigned rand_index = randomized_indices[orig_index];
                            renderPixel( rand_index / image_width, rand_index % image_width, 1 );
                        }
                        else { // TopToBottom
                            renderPixel( row, col, 1 );
                        }
                    }
                }
            }
        }
        else { // PositionSample
            for( unsigned int row = 0; row < image_height; ++row ) {
                if( image_flush_timer.elapsed() > min_flush_period_seconds ) {
                    printf("Flushing artifacts\n");
                    artifacts.flush();
                    image_flush_timer.start(); // reset timer
                }
                for( unsigned int col = 0; col < image_width; ++col ) {
                    if( traversal_order == Randomized ) {
                        unsigned orig_index = row * image_width + col;
                        unsigned rand_index = randomized_indices[orig_index];
                        renderPixel( rand_index / image_width, rand_index % image_width, rays_per_pixel );
                    }
                    else { // TopToBottom
                        renderPixel( row, col, rays_per_pixel );
                    }
                }
            }
        }
        endFrame( frame );
    }

    artifacts.flush();
    
    printf( "Intersection tests: AASlab: %lu Sphere: %lu TriangleMesh: %lu\n",
           AxisAlignedSlab::intersection_test_count,
           Sphere::intersection_test_count,
           TriangleMesh::intersection_test_count
           );
}

void ImageTracer::beginFrame( unsigned int frame_index )
{
    if( num_frames > 1 ) {
        // For non-loopable animation, anim_progress = 1.0 on the last frame
        if( loopable_animations ) {
            anim_progress = (float) frame_index / num_frames;
        }
        else {
            anim_progress = (float) frame_index / (num_frames - 1);
        }
    }
    else {
        anim_progress = 0.0f;
    }

    if( camera_transform_cb ) {
        camera.transform = camera_transform_cb( anim_progress );
    }
    //camera.transform.print();

    scene->updateAnim( anim_progress );
}

void ImageTracer::endFrame( unsigned int frame_index )
{
    if( frame_index < num_frames - 1 )
        artifacts.startNewFrame();
}

void ImageTracer::renderPixel( unsigned int row, unsigned int col, unsigned int num_rays )
{
    beginRenderPixel( row, col );
    for( unsigned int ray_index = 0; ray_index < num_rays; ++ray_index ) {
        tracePixelRay( row, col, ray_index );
    }
    endRenderPixel( row, col );
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
    pixel_render_timer.stop();
    artifacts.accumPixelTime( row, col, pixel_render_timer.elapsed() );
    if( traversal_nesting == PositionSample ) {
        pixel_color.scale( 1.0f / rays_per_pixel );
    }
    artifacts.accumPixelColorRGB( row, col, pixel_color.r, pixel_color.g, pixel_color.b );
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

    if( !hit ) {
        hit = scene->intersectEnvMap( ray, intersection );
    }

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


