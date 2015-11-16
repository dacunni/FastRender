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

void Plot2D::pointColor( float r, float g, float b )
{
    point_color = Magick::ColorRGB( r, g, b );
}

void Plot2D::strokeColor( float r, float g, float b )
{
    image->strokeColor( Magick::ColorRGB( r, g, b ) );
}

void Plot2D::fillColor( float r, float g, float b )
{
    image->fillColor( Magick::ColorRGB( r, g, b ) );
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

void Plot2D::drawRect( float x1, float y1, float x2, float y2 )
{
    image->draw( Magick::DrawableRectangle( imgx( x1 ), imgy( y1 ),
                                            imgx( x2 ), imgy( y2 ) ) );
}

void Plot2D::drawCircle( float x, float y, float radius )
{
    image->draw( Magick::DrawableCircle( imgx( x ), imgy( y ),
                                         imgx( x ), imgy( y + radius ) ) );
}

void Plot2D::drawAxes()
{
    float tick_size = 0.05;

    // x axis
    drawLine( xmin, 0.0, xmax, 0.0 );
    float xstep = 0.1;
    // TODO
//    for( int i = 0; i <= 10; i++ ) {
//        float tx = (xmax - xmin) / 10 * i;
//        drawLine( tx, 0.0, tx, tick_size );
//    }

    // y axis
    drawLine( 0.0, ymin, 0.0, ymax );
    float ystep = 0.1;
    // TODO
}


