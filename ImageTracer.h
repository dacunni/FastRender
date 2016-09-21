/*
 *  ImageTracer.h
 *  FastRender
 *
 *  Created by David Cunningham on 1/22/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _IMAGE_TRACER_H_
#define _IMAGE_TRACER_H_

#include "RandomNumberGenerator.h"
#include "SimpleCamera.h"
#include "Artifacts.h"
#include "Scene.h"
#include "Shader.h"
#include "Timer.h"

class ImageTracer
{
    public:
        ImageTracer( unsigned int w, unsigned int h,
                     unsigned int num_frames = 1,
                     unsigned int rays_per_pixel = 30 );
        virtual ~ImageTracer();

        void setCameraTransform( const Transform & xform ) { camera.transform = xform; }
        void setCameraTransform( const std::function<Transform(float)> & xform_cb ) { camera_transform_cb = xform_cb; }

        virtual void render();
        virtual void renderPixel( unsigned int row, unsigned int col );

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
        Shader * shader;
        unsigned int image_width;
        unsigned int image_height;
        unsigned int num_frames;
        unsigned int rays_per_pixel;

        // If true, will adjust animation interpolant to make animation loops seemless
        bool loopable_animations = false;

        // Randomize the order of pixel rendering within a frame, instead of raster order
        bool randomize_pixel_order = true;

        unsigned int flush_period_rows = 10;
        unsigned int min_flush_period_seconds = 5.0;

    protected:
        // Frame rendering
        // Blend factor from 0.0 to 1.0 throughout animation
        float anim_progress = 0.0f;
        std::function<Transform(float)> camera_transform_cb;

        // Pixel rendering
        Timer pixel_render_timer;
        RGBColor pixel_color;
        Vector4 pixel_normal;
        float pixel_distance;
        unsigned int num_hits;
};



#endif
