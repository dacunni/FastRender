#include <iostream>
#include <fstream>
#include <memory>

#include <sys/stat.h>
#include <stdio.h>
#include <math.h>

#define MAGICKCORE_QUANTUM_DEPTH 16
#define MAGICKCORE_HDRI_ENABLE 0
#include <Magick++.h>

#include "Matrix.h"
#include "Transform.h"
#include "Ray.h"
#include "Sphere.h"
#include "RandomNumberGenerator.h"
#include "Timer.h"

RandomNumberGenerator rng;

std::string output_path = "testoutput";
unsigned int plot_size = 500;
const unsigned int point_per_plot = 100000;

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

void testUniformSquare()
{
    Plot2D plot( output_path + "/random_square.png", plot_size, plot_size );

    for( auto i = 0; i < point_per_plot; i++ ) {
        float x = rng.uniformRange( -0.9, 0.9 );
        float y = rng.uniformRange( -0.9, 0.9 );
        plot.addPoint( x, y );
    }
}

void testUniformSurfaceUnitSphere()
{
    Plot2D plot( output_path + "/random_sphere.png", plot_size, plot_size );
    float x = 0.0f, y = 0.0f, z = 0.0f;

    for( auto i = 0; i < point_per_plot; i++ ) {
        rng.uniformSurfaceUnitSphere( x, y, z );
        plot.addPoint( x, y );
    }
}

void testUniformSurfaceUnitHalfSphere()
{
    Plot2D plot( output_path + "/random_half_sphere.png", plot_size, plot_size );
    Vector4 dir( 1.0, 1.0, 1.0 ), v;

    for( auto i = 0; i < point_per_plot; i++ ) {
        rng.uniformSurfaceUnitHalfSphere( dir, v );
        plot.addPoint( v.x, v.y );
    }
}

void testUniformConeDirection()
{
    Plot2D plot( output_path + "/random_cone.png", plot_size, plot_size );
    Vector4 dir( 1.0, 1.0, 1.0 ), v;
    const float angle = 0.3f;

    for( auto i = 0; i < point_per_plot; i++ ) {
        rng.uniformConeDirection( dir, angle, v );
        plot.addPoint( v.x, v.y );
    }
}

void testUniformVolumeUnitSphere()
{
    Plot2D plot( output_path + "/random_volume_sphere.png", plot_size, plot_size );
    float x = 0.0f, y = 0.0f, z = 0.0f;

    for( auto i = 0; i < point_per_plot; i++ ) {
        rng.uniformVolumeUnitSphere( x, y, z );
        plot.addPoint( x, y );
    }
}

int main (int argc, char * const argv[]) 
{
    printf("RNG Tests\n");
    fflush(stdout);

    mkdir(output_path.c_str(), 0777);

    Timer total_run_timer;
    total_run_timer.start();

    rng.seedCurrentTime();

    testUniformSquare();
    testUniformSurfaceUnitSphere();
    testUniformSurfaceUnitHalfSphere();
    testUniformConeDirection();

    testUniformVolumeUnitSphere();
    
    total_run_timer.stop();
    printf("Done - Run time = %f seconds\n", total_run_timer.elapsed());
    fflush(stdout);
    return 0;
}

