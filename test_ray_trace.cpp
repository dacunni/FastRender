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

// ------------------------------------------------------------ 
// Fresnel
// ------------------------------------------------------------ 
void testFresnelDialectric1()
{
    Plot2D plot( output_path + "/fresnel_dialectric_1.png", plot_size, plot_size,
                 -M_PI/2.0, M_PI/2.0, -M_PI/2.0, M_PI/2.0 );

    float n1 = 1.0, n2 = 1.4;

    float da = 0.05;
    for( float angle1 = -M_PI / 2.0; angle1 <= M_PI / 2.0; angle1 += da ) {
        for( float angle2 = -M_PI / 2.0; angle2 <= M_PI / 2.0; angle2 += da ) {
            float f = fresnelDialectric( cos(angle1+da/2.0), cos(angle2+da/2.0), n1, n2 );
            plot.strokeColor( f, f, f );
            plot.fillColor( f, f, f );
            plot.addPoint( angle1, angle2 );
            plot.drawRect( angle1, angle2, angle1 + da , angle2 + da );
        }
    }
}

void testFresnelDialectric2()
{
    Plot2D plot( output_path + "/fresnel_dialectric_2.png", plot_size, plot_size,
                 0, 1, 0, 1 );

    float n1 = 1.0, n2 = 1.4;

    float dc = 0.01;
    for( float cos1 = 0.0; cos1 <= 1.0; cos1 += dc ) {
        for( float cos2 = 0.0; cos2 <= 1.0; cos2 += dc ) {
            float f = fresnelDialectric( cos1+dc/2.0, cos2+dc/2.0, n1, n2 );
            plot.strokeColor( f, f, f );
            plot.fillColor( f, f, f );
            plot.addPoint( cos1, cos2 );
            plot.drawRect( cos1, cos2, cos1 + dc, cos2 + dc );
        }
    }
}

// ------------------------------------------------------------ 
// Snell's Law
// ------------------------------------------------------------ 
void testSnellAnglesHelper( float n1, float n2, Plot2D & plot )
{
    float da = 0.05;
    float last_angle1 = -M_PI / 2.0;
    float last_angle2 = snellsLawAngle( n1, last_angle1, n2 );
    for( float angle1 = -M_PI / 2.0; angle1 <= M_PI / 2.0; angle1 += da ) {
        float angle2 = snellsLawAngle( n1, angle1, n2 );
        plot.strokeColor( 0, 0, 0 );
        plot.fillColor( 0, 0, 0 );
        plot.drawLine( last_angle1, last_angle2, angle1, angle2 );
        last_angle1 = angle1;
        last_angle2 = angle2;
    }
}

void testSnellAngles()
{
    Plot2D plot( output_path + "/snell_angles.png", plot_size, plot_size,
                 -M_PI/2.0, M_PI/2.0, -M_PI/2.0, M_PI/2.0 );
    plot.drawAxes();
    for( float n2 = 1.0; n2 < 3.0; n2 += 0.1) {
        testSnellAnglesHelper( 1.0, n2, plot );
    }
}


// ------------------------------------------------------------ 
// Simple ray intersection
// ------------------------------------------------------------ 
void testRayIntersect()
{
    Plot2D plot( output_path + "/ray_intersect.png", plot_size, plot_size,
                 -1.0, 1.0, -1.0, 1.0 );
    
    // Slab with top on z=0 plane
    AxisAlignedSlab slab( -1.0, -1.0, -1.0, 1.0, 0.0, 1.0 );
    plot.drawLine( slab.xmin, slab.ymax, slab.xmax, slab.ymax );

    auto o = Vector4( -0.95, 0.95, 0.0 );
    auto d = Vector4( 1.0, -1.0, 0.0 ).normalized();
    auto tip = add( o, d );
    auto ray = Ray( o, d );
    RayIntersection ri;

    plot.drawCircle( o.x, o.y, 0.01 );
    plot.drawLine( o, tip );

    if( slab.intersect( ray, ri ) ) {
        auto &p = ri.position;
        //plot.addPoint<Vector4>( p );
        plot.drawCircle( p.x, p.y, 0.01 );
        plot.drawLine( p, add( p, ri.normal) );
        plot.drawLine( p, add( p, mirror( d.negated(), ri.normal ) ) );
    }
}

void testReflectAnglesHelper( const Vector4 & o, Traceable & obj, Plot2D & plot )
{
    auto at = Vector4( 0.0, 0.0, 0.0 );
    auto d = subtract( at, o ).normalized();
    auto tip = add( o, d );
    auto ray = Ray( o, d );
    RayIntersection ri;

    plot.drawCircle( o.x, o.y, 0.01 );
    plot.drawLine( o, tip );

    if( obj.intersect( ray, ri ) ) {
        auto &p = ri.position;
        //plot.addPoint<Vector4>( p );
        plot.drawCircle( p.x, p.y, 0.01 );
        plot.drawLine( p, add( p, ri.normal) );
        plot.drawLine( p, add( p, mirror( d.negated(), ri.normal ) ) );
    }
}

void testReflectAngles()
{
    Plot2D plot( output_path + "/reflect_angles.png", plot_size, plot_size,
                 -2.0, 2.0, -2.0, 2.0 );
    
    // Slab with top on z=0 plane
    AxisAlignedSlab slab( -2.0, -2.0, -2.0, 2.0, 0.0, 2.0 );
    plot.drawLine( slab.xmin, slab.ymax, slab.xmax, slab.ymax );

    auto o = Vector4( -0.95, 0.95, 0.0 );

    float radius = 2.0;
    int nsteps = 30;
    for( int i = 1; i <= nsteps; i++ ) {
        float alpha = (float) i / (nsteps + 1);
        float angle = (1.0 - alpha) * M_PI + alpha * M_PI / 2.0;
        plot.strokeColor( alpha, 1.0, 1.0 - alpha );
        testReflectAnglesHelper( Vector4( radius*cos(angle), radius*sin(angle), 0.0 ), slab, plot );
    }
}


// ------------------------------------------------------------ 
// Camera ray generation
// ------------------------------------------------------------ 
void drawCameraPoints( Plot2D & plot, int w, int h, Vector4 *points )
{
    // vectors
    for( int ri = 0; ri < h; ri++ ) {
        for( int ci = 0; ci < w; ci++ ) {
            Vector4 v = points[ri * w + ci];
            plot.strokeColor( 0, 0, 0 );
            plot.drawLine( Vector4( 0, 0, 0 ), v );
        }
    }

    // endpoints
    for( int ri = 0; ri < h; ri++ ) {
        for( int ci = 0; ci < w; ci++ ) {
            Vector4 v = points[ri * w + ci];
            plot.fillColor( (float) ri / h, (float) ci / w, 1.0 );
            plot.strokeColor( 0, 0, 0 );
            plot.drawCircle( v.x, v.y, 0.01 );
        }
    }
}

void testSimpleCameraNoJitter()
{
    const int w = 20, h = 20;
    Plot2D plot( output_path + "/simple_camera_no_jitter.png", plot_size, plot_size,
                 -1.0, 1.0, -1.0, 1.0 );
    SimpleCamera camera( rng,
                         -1.0, 1.0, -1.0, 1.0,
                         w, h );
    camera.jitter_rays = false;

    Vector4 points[h*w];
    for( int ri = 0; ri < h; ri++ ) {
        for( int ci = 0; ci < w; ci++ ) {
            points[ri * w + ci] = camera.vectorThrough( ri, ci );
        }
    }

    drawCameraPoints( plot, w, h, points );
}

void testSimpleCamera()
{
    const int w = 20, h = 20;
    Plot2D plot( output_path + "/simple_camera.png", plot_size, plot_size,
                 -1.0, 1.0, -1.0, 1.0 );
    SimpleCamera camera( rng,
                         -1.0, 1.0, -1.0, 1.0,
                         w, h );
    camera.jitter_rays = true;

    Vector4 points[h*w];
    for( int ri = 0; ri < h; ri++ ) {
        for( int ci = 0; ci < w; ci++ ) {
            points[ri * w + ci] = camera.vectorThrough( ri, ci );
        }
    }

    drawCameraPoints( plot, w, h, points );
}

// ------------------------------------------------------------ 
// Test runner
// ------------------------------------------------------------ 
int main (int argc, char * const argv[]) 
{
    printf("Ray Trace Tests\n");
    fflush(stdout);

    mkdir(output_path.c_str(), 0777);

    Timer total_run_timer;
    total_run_timer.start();

    rng.seedCurrentTime();

    // Tests
#if 1
    testRayIntersect();
    testReflectAngles();
    testSnellAngles();
    testFresnelDialectric1();
    testFresnelDialectric2();
    testSimpleCameraNoJitter();
    testSimpleCamera();
#else
#endif
    
    total_run_timer.stop();
    printf("Done - Run time = %f seconds\n", total_run_timer.elapsed());
    fflush(stdout);
    return 0;
}

