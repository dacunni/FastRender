//
//  Plot2D.h
//  FastRender
//
//  Created by David Cunningham on 4/29/15.
//
//
#ifndef __FastRender__Plot2D__
#define __FastRender__Plot2D__

#include <memory>
#include <string>

#define MAGICKCORE_QUANTUM_DEPTH 16
#define MAGICKCORE_HDRI_ENABLE 0
#include <Magick++.h>

class Plot2D
{
    public:
        Plot2D( const std::string & fn, unsigned int w, unsigned int h,
                float xmin = -1.0, float xmax = 1.0, float ymin = -1.0, float ymax = 1.0 );
        ~Plot2D();

        void addPoint( float x, float y );

        unsigned int width, height;
        float xmin, xmax;
        float ymin, ymax;
        std::unique_ptr<Magick::Image> image;
        Magick::ColorRGB point_color;
        std::string filename;
};

#endif /* defined(__FastRender__Plot2D__) */
