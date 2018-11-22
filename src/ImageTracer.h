#ifndef _IMAGE_TRACER_H_
#define _IMAGE_TRACER_H_

#include <thread>

#include "RandomNumberGenerator.h"
#include "SimpleCamera.h"
#include "Artifacts.h"
#include "Scene.h"
#include "Shader.h"
#include "Timer.h"
#include "PreviewWindow.h"

class Logger;

class ImageTracer
{
    public:
        ImageTracer( unsigned int w, unsigned int h,
                     unsigned int num_frames = 1,
                     unsigned int rays_per_pixel = 30 );
        ~ImageTracer();

        void setCameraTransform( const Transform & xform ) { camera->transform = xform; }
        void setCameraTransform( const std::function<Transform(float)> & xform_cb ) { camera_transform_cb = xform_cb; }

        void setAnimationCallback( const std::function<void(float)> & anim_cb ) { animation_cb = anim_cb; }

        // Record of a single ray trace
        struct HitRecord {
            RGBColor color;
            Vector4  normal;
            float    distance = FLT_MAX;
            bool     hit      = false;
        };

        // Record of a series of ray traces. Accumulates color and squared color
        struct MultiHitRecord {
            ProcessorTimer  timer;
            RGBColor        color_sum;
            RGBColor        color_sq_sum;
            Vector4         normal;
            float           distance = 0.0f;
            unsigned int    num_hits = 0;

            float maxColorChannelVariance() const {
                if(num_hits < 1)
                    return 0.0f;
                auto color = color_sum / float(num_hits);
                auto color_sq = color_sq_sum / float(num_hits);
                auto color_var = color_sq - color * color;
                return color_var.channelMax();
            }
        };

        void render();
        void renderPixel( unsigned int row, unsigned int col,
                          unsigned int num_rays );

        void beginFrame( unsigned int frame_index );
        void endFrame( unsigned int frame_index );

        HitRecord tracePixelRay( unsigned int row, unsigned int col );
        void tracePixelRay( unsigned int row, unsigned int col,
                                    MultiHitRecord & multirec );
        MultiHitRecord tracePixelRays( unsigned int row, unsigned int col, unsigned int num_rays );

        RandomNumberGenerator rng;
        std::shared_ptr<Camera> camera;
        Artifacts artifacts;
        Scene * scene = nullptr;
        Shader * shader = nullptr;
        unsigned int num_frames;
        unsigned int rays_per_pixel;

        // If true, will adjust animation interpolant to make animation loops seemless
        bool loopable_animations = false;

        enum TraversalOrder { TopToBottom, Randomized };
        TraversalOrder traversal_order = TopToBottom;
        enum TraversalNesting { PositionSample, SamplePosition, TilePositionSample };
        TraversalNesting traversal_nesting = TilePositionSample;
        // FIXME: It seems we aren't accumulating pixels correctly in SamplePosition traversal order
        //TraversalNesting traversal_nesting = SamplePosition;

        //float min_flush_period_seconds = 5.0;
        float min_flush_period_seconds = 60.0;

        bool show_preview_window = true;

    protected:
        // Frame rendering
        // Blend factor from 0.0 to 1.0 throughout animation
        float anim_progress = 0.0f;
        // A callback function for animating the camera transform
        std::function<Transform(float)> camera_transform_cb;
        // A callback function for updating state for each frame
        std::function<void(float)> animation_cb;

        void renderThread();
        std::thread render_thread;

        PreviewWindow preview_window;

        Logger & logger;
};



#endif
