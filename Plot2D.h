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

        void pointColor( float r, float g, float b );
        void strokeColor( float r, float g, float b );
        void fillColor( float r, float g, float b );

        void addPoint( float x, float y );
        void drawLine( float x1, float y1, float x2, float y2 );
        void drawCircle( float x, float y, float radius );
        void drawAxes();

        template<typename VecType>
        void addPoint( const VecType & v );
        template<typename VecType>
        void drawLine( const VecType & u, const VecType & v );
        template<typename VecType>
        void drawCircle( const VecType & v, float radius );

    private:
        float imgx( float x );
        float imgy( float y );

        unsigned int width, height;
        float xmin, xmax;
        float ymin, ymax;
        std::unique_ptr<Magick::Image> image;
        Magick::ColorRGB point_color;
        std::string filename;
};

#include "Plot2D.hpp"

#endif /* defined(__FastRender__Plot2D__) */
