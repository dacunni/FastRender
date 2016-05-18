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

// Low index to high index
void testFresnelDialectric3()
{
    Plot2D plot( output_path + "/fresnel_dialectric_3.png", plot_size, plot_size,
                 0, 1, 0, 1 );

    float n1 = 1.0, n2 = 1.4;

    float dc = 0.01;

    float last_cos1 = 0.0;
    float last_f = fresnelDialectricSnell( 0.0, n1, n2 );
    for( float cos1 = 0.0; cos1 <= 1.0; cos1 += dc ) {
        float f = fresnelDialectricSnell( cos1, n1, n2 );
        printf("cos1 = %f f = %f\n", cos1, f); 
        plot.strokeColor( 0, 0, 0 );
        plot.drawLine( last_cos1, last_f, cos1, f );
        last_cos1 = cos1;
        last_f = f;
    }
    last_cos1 = 0.0;
    last_f = fresnelDialectricParallelSnell( 0.0, n1, n2 );
    for( float cos1 = 0.0; cos1 <= 1.0; cos1 += dc ) {
        float f = fresnelDialectricParallelSnell( cos1, n1, n2 );
        printf("cos1 = %f f = %f\n", cos1, f); 
        plot.strokeColor( 0, 0, 1 );
        plot.drawLine( last_cos1, last_f, cos1, f );
        last_cos1 = cos1;
        last_f = f;
    }
    last_cos1 = 0.0;
    last_f = fresnelDialectricPerpendicularSnell( 0.0, n1, n2 );
    for( float cos1 = 0.0; cos1 <= 1.0; cos1 += dc ) {
        float f = fresnelDialectricPerpendicularSnell( cos1, n1, n2 );
        printf("cos1 = %f f = %f\n", cos1, f); 
        plot.strokeColor( 1, 0, 0 );
        plot.drawLine( last_cos1, last_f, cos1, f );
        last_cos1 = cos1;
        last_f = f;
    }
}

// High index to low index
void testFresnelDialectric4()
{
    Plot2D plot( output_path + "/fresnel_dialectric_4.png", plot_size, plot_size,
                 0, 1, 0, 1 );

    float n1 = 1.4, n2 = 1.0;

    float dc = 0.01;

    float last_cos1 = 0.0;
    float last_f = fresnelDialectricSnell( 0.0, n1, n2 );
    for( float cos1 = 0.0; cos1 <= 1.0; cos1 += dc ) {
        float f = fresnelDialectricSnell( cos1, n1, n2 );
        printf("cos1 = %f f = %f\n", cos1, f); 
        plot.strokeColor( 0, 0, 0 );
        plot.drawLine( last_cos1, last_f, cos1, f );
        last_cos1 = cos1;
        last_f = f;
    }
    last_cos1 = 0.0;
    last_f = fresnelDialectricParallelSnell( 0.0, n1, n2 );
    for( float cos1 = 0.0; cos1 <= 1.0; cos1 += dc ) {
        float f = fresnelDialectricParallelSnell( cos1, n1, n2 );
        printf("cos1 = %f f = %f\n", cos1, f); 
        plot.strokeColor( 0, 0, 1 );
        plot.drawLine( last_cos1, last_f, cos1, f );
        last_cos1 = cos1;
        last_f = f;
    }
    last_cos1 = 0.0;
    last_f = fresnelDialectricPerpendicularSnell( 0.0, n1, n2 );
    for( float cos1 = 0.0; cos1 <= 1.0; cos1 += dc ) {
        float f = fresnelDialectricPerpendicularSnell( cos1, n1, n2 );
        printf("cos1 = %f f = %f\n", cos1, f); 
        plot.strokeColor( 1, 0, 0 );
        plot.drawLine( last_cos1, last_f, cos1, f );
        last_cos1 = cos1;
        last_f = f;
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

// ----- Reflection -----

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

// ----- Refraction -----

void testRefractAnglesHelper( const Vector4 & o, Traceable & obj, Plot2D & plot,
                              float n1, float n2 )
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
        //plot.drawCircle( p.x, p.y, 0.01 );
        //plot.drawLine( p, add( p, ri.normal) );

        Vector4 in = d.negated();

        Vector4 refracted = refract( in, ri.normal, n1, n2 );

        //r = scale(r, fresnelDialectric( dot( in, ri.normal ),
        //                                dot( r, ri.normal.negated() ),
        //                                n1, n2 ) );
        float cos_in = dot( in, ri.normal );
        float cos_out = dot( refracted, ri.normal.negated() );
        float f = fresnelDialectric( cos_in, cos_out, n1, n2 );
        refracted = scale(refracted, 1.0 - f);

        // Mark rays that saw total internal reflection
        if( refracted.magnitude() < 0.001 ) {
            plot.drawCircle( o.x, o.y, 0.03 );
        }

        plot.drawLine( p, add( p, refracted ) );

        // Draw reflected ray
        if( refracted.magnitude() < 1.0 ) {
            Vector4 reflected = mirror( in, ri.normal );
            reflected = scale( reflected, f );
            plot.drawLine( p, add( p, reflected ) );
        }
    }
}

void testRefractForIndices( Plot2D & plot, float n1, float n2 )
{
    // Slab with top on z=0 plane
    AxisAlignedSlab slab( -2.0, -2.0, -2.0, 2.0, 0.0, 2.0 );
    plot.drawLine( slab.xmin, slab.ymax, slab.xmax, slab.ymax );

    //float radius = 2.0;
    float radius = 1.5;
    int nsteps = 40;

    for( int i = 1; i <= nsteps; i++ ) {
        float alpha = (float) i / (nsteps + 2);
        float angle = (1.0 - alpha) * M_PI + alpha * M_PI / 2.0;
        if( i == nsteps / 2 )
            plot.strokeColor( 0.0, 1.0, 0.0 );
        else
            plot.strokeColor( alpha, 0.0, 1.0 - alpha );
        testRefractAnglesHelper( Vector4( radius*cos(angle), radius*sin(angle), 0.0 ), slab, plot, n1, n2 );
    }
}

// Test refraction going from lower to higher index of refraction
void testRefractAnglesLowHigh()
{
    Plot2D plot( output_path + "/refract_angles_low_high.png", plot_size, plot_size,
                 -1.6, 1.6, -1.6, 1.6 );
    testRefractForIndices( plot, 1.1, 1.4 );
}

// Test refraction going from higher to lower index of refraction
// Should see effects of total internal reflection
void testRefractAnglesHighLow()
{
    Plot2D plot( output_path + "/refract_angles_high_low.png", plot_size, plot_size,
                 -1.6, 1.6, -1.6, 1.6 );
    testRefractForIndices( plot, 1.4, 1.1 );
}

void testRefractAnglesEqualIndex()
{
    Plot2D plot( output_path + "/refract_angles_equal_index.png", plot_size, plot_size,
                 -1.6, 1.6, -1.6, 1.6 );
    testRefractForIndices( plot, 1.2, 1.2 );
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
    testRefractAnglesLowHigh();
    testRefractAnglesHighLow();
    testRefractAnglesEqualIndex();
    testFresnelDialectric1();
    testFresnelDialectric2();
    testFresnelDialectric3();
    testFresnelDialectric4();
    testSimpleCameraNoJitter();
    testSimpleCamera();
    testRefractAnglesLowHigh();
    testRefractAnglesHighLow();
    testRefractAnglesEqualIndex();
#else
#endif
    
    total_run_timer.stop();
    printf("Done - Run time = %f seconds\n", total_run_timer.elapsed());
    fflush(stdout);
    return 0;
}
