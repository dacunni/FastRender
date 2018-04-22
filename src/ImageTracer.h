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

class ImageTracer
{
    public:
        ImageTracer( unsigned int w, unsigned int h,
                     unsigned int num_frames = 1,
                     unsigned int rays_per_pixel = 30 );
        virtual ~ImageTracer();

        void setCameraTransform( const Transform & xform ) { camera.transform = xform; }
        void setCameraTransform( const std::function<Transform(float)> & xform_cb ) { camera_transform_cb = xform_cb; }

        void setAnimationCallback( const std::function<void(float)> & anim_cb ) { animation_cb = anim_cb; }

        virtual void render();
        virtual void renderPixel( unsigned int row, unsigned int col,
                                  unsigned int num_rays );

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

        // Pixel rendering
        Timer pixel_render_timer;
        RGBColor pixel_color;
        Vector4 pixel_normal;
        float pixel_distance;
        unsigned int num_hits;

        void renderThread();
        std::thread render_thread;

        PreviewWindow preview_window;
};



#endif
