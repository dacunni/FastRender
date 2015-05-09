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

float Plot2D::imgx( float x )
{
    return (float) (width - 1) * (x - xmin) / (xmax - xmin);
}

float Plot2D::imgy( float y )
{
    return (float) (height - 1) * (1.0 - (y - ymin) / (ymax - ymin));
}

void Plot2D::addPoint( float x, float y )
{
    image->pixelColor( imgx( x ), imgy( y ), point_color );
}

void Plot2D::drawLine( float x1, float y1, float x2, float y2 )
{
    image->draw( Magick::DrawableLine( imgx( x1 ), imgy( y1 ),
                                       imgx( x2 ), imgy( y2 ) ) );
}

