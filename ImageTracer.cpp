#include <iostream>

#include "ImageTracer.h"
#include "Logger.h"

// for intersection test logging
#include "AxisAlignedSlab.h"
#include "Sphere.h"
#include "TriangleMesh.h"

#define SUPPRESS_FIREFLIES 1

ImageTracer::ImageTracer( unsigned int w, unsigned int h,
                          unsigned int nframes,
                          unsigned int rayspp )
    : rng(),
      artifacts( w, h ),
      rays_per_pixel( rayspp ),
      num_frames( nframes ),
      preview_window( artifacts ),
      logger( getLogger() )
{
    camera = std::make_shared<SimpleCamera>( 0.3, 0.3, w, h );
}

ImageTracer::~ImageTracer()
{

}

void ImageTracer::render()
{
#if 1
    render_thread = std::thread(&ImageTracer::renderThread, this);
    if( show_preview_window ) {
        preview_window.init("Preview");
        preview_window.start();
    }
    // Probably never get here when preview window takes over the main thread
    render_thread.join();
#elif 1
    render_thread = std::thread(&ImageTracer::renderThread, this);
    render_thread.join();
#else
    renderThread();
#endif
}

void ImageTracer::renderThread()
{
    WallClockTimer image_flush_timer;
    ProcessorTimer processor_timer;
    WallClockTimer wall_clock_timer;

    const unsigned int image_width = camera->imageWidth();
    const unsigned int image_height = camera->imageHeight();

    processor_timer.start();
    wall_clock_timer.start();

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

    artifacts.startNewFrame();

    logger.normalf("Tracing scene :  %u x %u frm %u rpp %u",
                  image_width, image_height,
                  num_frames, rays_per_pixel);

    image_flush_timer.start();
    for( unsigned int frame = 0; frame < num_frames; ++frame ) {
        logger.normalf("FRAME %4d / %4d", frame + 1, num_frames);
        beginFrame( frame );
        if( traversal_nesting == SamplePosition ) {
            for( unsigned int sample_index = 0; sample_index < rays_per_pixel; sample_index++ ) {
                for( unsigned int row = 0; row < image_height; ++row ) {
                    if( image_flush_timer.elapsed() > min_flush_period_seconds ) {
                        logger.normalf("Flushing artifacts (progress: frame = %.2f %% anim = %.2f %% elapsed = %f)",
                                       (float) (image_height * sample_index + row) / (image_height * rays_per_pixel) * 100.0f,
                                       num_frames > 1 ? (float) frame / (num_frames - 1) * 100.0f : 0.0f,
                                       image_flush_timer.elapsed());
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
        else if( traversal_nesting == TilePositionSample ) {
            unsigned int tileSize = 8;
            unsigned int numGridRows = (image_height + tileSize - 1) / tileSize;
            unsigned int numGridCols = (image_width + tileSize - 1) / tileSize;

            for( unsigned int gridRow = 0; gridRow < numGridRows; ++gridRow ) {
                unsigned int numTileRows = (gridRow == numGridRows - 1 && image_height % tileSize != 0)
                    ? image_height % tileSize : tileSize;
                for( unsigned int gridCol = 0; gridCol < numGridCols; ++gridCol ) {
                    unsigned int numTileCols = (gridCol == numGridCols - 1 && image_width % tileSize != 0)
                        ? image_width % tileSize : tileSize;

                    if( image_flush_timer.elapsed() > min_flush_period_seconds ) {
                        logger.normalf("Flushing artifacts (progress: frame = %.2f %% anim = %.2f %% elapsed = %f)",
                                       (float) gridRow / numGridRows * 100.0f,
                                       num_frames > 1 ? (float) frame / (num_frames - 1) * 100.0f : 0.0f,
                                       image_flush_timer.elapsed());
                        artifacts.flush();
                        image_flush_timer.start(); // reset timer
                    }

                    for( unsigned int tileRow = 0; tileRow < numTileRows; tileRow++ ) {
                        for( unsigned int tileCol = 0; tileCol < numTileCols; tileCol++ ) {
                            unsigned int row = gridRow * tileSize + tileRow;
                            unsigned int col = gridCol * tileSize + tileCol;
                            renderPixel( row, col, rays_per_pixel );
                        }
                    }
                }
            }
        }
        else if( traversal_nesting == PositionSample ) {
            for( unsigned int row = 0; row < image_height; ++row ) {
                if( image_flush_timer.elapsed() > min_flush_period_seconds ) {
                    logger.normalf("Flushing artifacts (progress: frame = %.2f %% anim = %.2f %% elapsed = %f)",
                                   (float) row / image_height * 100.0f,
                                   num_frames > 1 ? (float) frame / (num_frames - 1) * 100.0f : 0.0f,
                                   image_flush_timer.elapsed());
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
        else {
            logger.errorf("ERROR: Unknown traversal nesting %d", (int) traversal_nesting);
        }
        endFrame( frame );
    }

    artifacts.flush();
    processor_timer.stop();
    wall_clock_timer.stop();
    
    logger.normalf("Intersection Complete, tests: AASlab: %lu Sphere: %lu TriangleMesh: %lu",
                   AxisAlignedSlab::intersection_test_count,
                   Sphere::intersection_test_count,
                   TriangleMesh::intersection_test_count
                  );
    logger.normal() << "Trace time: "
        << wall_clock_timer.elapsed() << " s (wall), "
        << processor_timer.elapsed() << " s (proc)";
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
        camera->transform = camera_transform_cb( anim_progress );
    }
    //camera.transform.print();

    if( animation_cb ) {
        animation_cb( anim_progress );
    }

    scene->updateAnim( anim_progress );
}

void ImageTracer::endFrame( unsigned int frame_index )
{
    if( frame_index < num_frames - 1 )
        artifacts.startNewFrame();
}

void ImageTracer::renderPixel( unsigned int row, unsigned int col, unsigned int num_rays )
{
    // Intersect rays with scene to get color and other measurements
    auto multirec = tracePixelRays( row, col, num_rays );

#if SUPPRESS_FIREFLIES // FIXME: HACK - Trying to deal with fireflies by retrying if we see a lot of variance
    const float variance_threhshold = 5.0f;
    float pixel_variance = multirec.maxColorChannelVariance();
    if( pixel_variance > variance_threhshold ) {
        //logger.debugf("FF suppress: Variance exceeded (%.2f > %.2f) at (%u, %u), retrying", pixel_variance, variance_threhshold, row, col);
        multirec = tracePixelRays( row, col, num_rays );
    }
#endif // SUPPRESS_FIREFLIES

    // Update output artifacts
    artifacts.setAccumPixelColor( row, col, multirec.color_sum, multirec.color_sq_sum, num_rays );
    artifacts.setPixelTime( row, col, multirec.timer.elapsed() );
    artifacts.setPixelNormal( row, col, multirec.normal );
    artifacts.setPixelDepth( row, col, multirec.distance );
}

ImageTracer::MultiHitRecord
ImageTracer::tracePixelRays( unsigned int row, unsigned int col, unsigned int num_rays )
{
    MultiHitRecord multirec;

    multirec.timer.start();
    for( unsigned int ray_index = 0; ray_index < num_rays; ++ray_index ) {
        tracePixelRay( row, col, multirec );
    }
    multirec.timer.stop();

    return multirec;
}

ImageTracer::HitRecord
ImageTracer::tracePixelRay( unsigned int row, unsigned int col )
{
    HitRecord rec;

    Ray ray = camera->rayThrough( rng, row, col );
    RayIntersection intersection = RayIntersection();

    rec.hit = scene->intersect( ray, intersection );

    if( !rec.hit ) {
        rec.hit = scene->intersectEnvMap( ray, intersection );
    }

    if( rec.hit ) {
        rec.normal = intersection.normal;
        rec.distance = intersection.distance;

        if( intersection.distance != FLT_MAX ) {
            shader->shade( *scene, rng, intersection );
        }
        rec.color = intersection.sample.color;
    }

    return rec;
}

void ImageTracer::tracePixelRay( unsigned int row, unsigned int col,
                                 MultiHitRecord & multirec  )
{
    auto rec = tracePixelRay(row, col);

    if( rec.hit ) {
        multirec.num_hits++;
        if( multirec.num_hits == 1) {
            multirec.normal = rec.normal;
            multirec.distance = rec.distance;
        }
        multirec.color_sum.accum( rec.color );
        multirec.color_sq_sum.accum( rec.color * rec.color );
    }
}


