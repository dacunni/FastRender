#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>

#include <sys/stat.h>
#include <stdio.h>
#include <math.h>

#include "FastRender.h"

RandomNumberGenerator rng;
std::string output_path = "testoutput";
unsigned int plot_size = 500;
const unsigned int points_per_plot = 100000;

void testUniformSquare()
{
    Plot2D plot( output_path + "/random_square.png", plot_size, plot_size );

    for( auto i = 0; i < points_per_plot; i++ ) {
        float x = rng.uniformRange( -0.9, 0.9 );
        float y = rng.uniformRange( -0.9, 0.9 );
        plot.addPoint( x, y );
    }
}

void testUniformCircle()
{
    Plot2D plot( output_path + "/random_circle.png", plot_size, plot_size );

    float x, y;
    for( auto i = 0; i < points_per_plot; i++ ) {
        rng.uniformUnitCircle( x, y );
        plot.addPoint( x, y );
    }
}

void testUniformSurfaceUnitSphere()
{
    Plot2D plot( output_path + "/random_sphere.png", plot_size, plot_size );
    float x = 0.0f, y = 0.0f, z = 0.0f;

    for( auto i = 0; i < points_per_plot; i++ ) {
        rng.uniformSurfaceUnitSphere( x, y, z );
        plot.addPoint( x, y );
    }
}

void testUniformSurfaceUnitHalfSphere()
{
    Plot2D plot( output_path + "/random_half_sphere.png", plot_size, plot_size );
    Vector4 dir( 1.0, 1.0, 1.0 ), v;

    for( auto i = 0; i < points_per_plot; i++ ) {
        rng.uniformSurfaceUnitHalfSphere( dir, v );
        plot.addPoint( v.x, v.y );
    }
}

void testUniformConeDirection()
{
    Plot2D plot( output_path + "/random_cone.png", plot_size, plot_size );
    Vector4 dir( 1.0, 1.0, 1.0 ), v;
    const float angle = 0.3f;

    for( auto i = 0; i < points_per_plot; i++ ) {
        rng.uniformConeDirection( dir, angle, v );
        plot.addPoint( v.x, v.y );
    }
}

void testUniformVolumeUnitSphere()
{
    Plot2D plot( output_path + "/random_volume_sphere.png", plot_size, plot_size );
    float x = 0.0f, y = 0.0f, z = 0.0f;

    for( auto i = 0; i < points_per_plot; i++ ) {
        rng.uniformVolumeUnitSphere( x, y, z );
        plot.addPoint( x, y );
    }
}

void testCosineDistribution()
{
    Plot2D plot_radial_hist( output_path + "/cosine_radial_hist.png", plot_size, plot_size );
    Plot2D plot_angle_hist( output_path + "/cosine_angle_hist.png", plot_size, plot_size, -0.5f * M_PI, 0.5f * M_PI );
    float x = 0.0f, y = 0.0f, z = 0.0f;

    // Build a histogram of random angles using a cosine distribution
    const int bins = 30;
    const float angle_range = 0.5f * M_PI;
    const float bin_width = angle_range / bins;
    int hist[bins] = {};

    for( auto i = 0; i < 1000000; i++ ) {
        float angle = rng.cosineQuarterWave();
        x = cosf( angle );
        y = sinf( angle );
        int bin = angle / angle_range * (bins - 1);
        hist[bin]++;
    }

    int *max_it = std::max_element( hist, hist + bins );
    int hist_max = *max_it;

    plot_radial_hist.drawAxes();
    plot_angle_hist.drawAxes();

    // Draw histogram bins
    plot_radial_hist.strokeColor( 1.0, 0.0, 0.0 );
    plot_angle_hist.strokeColor( 1.0, 0.0, 0.0 );
    for( auto i = 0; i < bins; i++ ) {
        float angle = (float) i / (bins - 1) * angle_range;
        float r = hist[i] / (float) hist_max;
        float x1 = cosf( angle ) * r;
        float y1 = sinf( angle ) * r;
        angle = (float) (i + 1) / (bins - 1) * angle_range;
        float x2 = cosf( angle ) * r;
        float y2 = sinf( angle ) * r;

        plot_radial_hist.drawLine( x1, y1, 0.0, 0.0 );
        plot_radial_hist.drawLine( x1, y1, x2, y2 );
        plot_radial_hist.drawLine( 0.0, 0.0, x2, y2 );
        plot_angle_hist.drawLine( angle, 0.0, angle, r );
        plot_angle_hist.drawLine( angle, r, angle + bin_width, r );
        plot_angle_hist.drawLine( angle + bin_width, r, angle + bin_width, 0.0 );
    }

    // Reference
    plot_angle_hist.strokeColor( 1.0, 0.0, 0.0 );
    for( auto i = 0; i < 100; i++ ) {
        float angle1 = (float) i / (bins - 1) * angle_range;
        float angle2 = (float) (i + 1) / (bins - 1) * angle_range;
        float r1 = cos(angle1);
        float r2 = cos(angle2);
        plot_angle_hist.drawLine( angle1, r1, angle2, r2 );
    }
}

void testCosineHalfSphere()
{
    Plot2D plot_down_z( output_path + "/cosine_half_sphere__down_z.png", plot_size, plot_size );
    Plot2D plot_down_y( output_path + "/cosine_half_sphere__down_y.png", plot_size, plot_size );
    Plot2D plot_rot( output_path + "/cosine_half_sphere__rot.png", plot_size, plot_size );
    Transform xform = makeRotation( 0.5, Vector4( 1.0f, 1.0f, 1.0f ) );
    Vector4 v, rot;

    for( auto i = 0; i < points_per_plot; i++ ) {
        rng.cosineUnitHalfSphere( v );
        plot_down_z.addPoint( v.x, v.y );
        plot_down_y.addPoint( v.x, v.z );
        rot = mult( xform.fwd, v );
        plot_rot.addPoint( rot.x, rot.y );
    }
}

void testBeckmanNDF(float roughness)
{
    char plot_down_z_name[1024], plot_down_y_name[1024], plot_rot_name[1024];
    sprintf(plot_down_z_name, "%s/beckman_ndf_%3.2f__down_z.png", output_path.c_str(), roughness);
    sprintf(plot_down_y_name, "%s/beckman_ndf_%3.2f__down_y.png", output_path.c_str(), roughness);
    //sprintf(plot_rot_name,    "%s/beckman_ndf_%3.2f__rot.png",    output_path.c_str(), roughness);
    Plot2D plot_down_z( plot_down_z_name, plot_size, plot_size );
    Plot2D plot_down_y( plot_down_y_name, plot_size, plot_size );
    //Plot2D plot_rot   ( plot_rot_name,    plot_size, plot_size );
    Transform xform = makeRotation( 0.5, Vector4( 1.0f, 1.0f, 1.0f ) );
    Vector4 v, rot;

    for( auto i = 0; i < points_per_plot; i++ ) {
        rng.beckmanNDF( roughness, v );
        plot_down_z.addPoint( v.x, v.y );
        plot_down_y.addPoint( v.x, v.z );
        //rot = mult( xform.fwd, v );
        //plot_rot.addPoint( rot.x, rot.y );
    }
}

void testBeckmanNDF()
{
    testBeckmanNDF(0.0f);
    testBeckmanNDF(0.02f);
    testBeckmanNDF(0.05f);
    testBeckmanNDF(0.1f);
    testBeckmanNDF(0.2f);
    testBeckmanNDF(0.3f);
    testBeckmanNDF(0.4f);
    testBeckmanNDF(0.6f);
    testBeckmanNDF(0.8f);
    testBeckmanNDF(1.0f);
    testBeckmanNDF(1.2f);
}

// TEMP - trying to see if i can uniformly generate a random number on a triangle
//        This works.
// TODO - try to generalize this to any triangle
void testBasicTriangle2D()
{
    Plot2D plot( output_path + "/test_basic_triangle_2d.png", plot_size, plot_size,
                 -0.1, 1.1, -0.1, 1.1 );

    float x, y;
    for( auto i = 0; i < points_per_plot; i++ ) {
        rng.uniformUnitTriangle2D( x, y );

        plot.addPoint( x, y );
    }
}

void testBasicTriangle3D()
{
    Plot2D plot( output_path + "/test_basic_triangle_3d.png", plot_size, plot_size,
                 -0.1, 1.1, -0.1, 1.1 );

    Vector4 v[3] = {
        Vector4( -0.9, 0.0, 0.0 ),
        Vector4( 1.0, 0.0, 0.9 ),
        Vector4( 0.8, 1.0, 0.0 )
    };

    Vector4 r;
    for( auto i = 0; i < points_per_plot; i++ ) {
        rng.uniformTriangle3D( v[0], v[1], v[2], r );

        plot.addPoint( r.x, r.y );
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

#if 0
    testUniformSquare();
    testUniformCircle();
    testUniformSurfaceUnitSphere();
    testUniformSurfaceUnitHalfSphere();
    testUniformConeDirection();
    testUniformVolumeUnitSphere();
    testCosineDistribution();
    testCosineHalfSphere();
    testBeckmanNDF();

    testBasicTriangle2D();
    testBasicTriangle3D();
#else
    testBeckmanNDF();
#endif
    
    total_run_timer.stop();
    printf("Done - Run time = %f seconds\n", total_run_timer.elapsed());
    fflush(stdout);
    return 0;
}

