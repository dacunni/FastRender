#include "Plot2D.h"

Plot2D::Plot2D( const std::string & fn, unsigned int w, unsigned int h,
                float xmin, float xmax, float ymin, float ymax )
    : filename(fn), width(w), height(h),
      xmin(xmin), xmax(xmax), ymin(ymin), ymax(ymax),
      image( new Magick::Image( Magick::Geometry( w, h ), "white" ) ),
      point_color( 0.0f, 0.0f, 0.0f )
{
    image->magick( "png" );
}

Plot2D::~Plot2D()
{
    image->write( filename );
}

void Plot2D::addPoint( float x, float y )
{
    float imgx = (float) width * (x - xmin) / (xmax - xmin);
    float imgy = (float) height * (1.0 - (y - ymin) / (ymax - ymin));

    image->pixelColor( imgx, imgy, point_color );
}

