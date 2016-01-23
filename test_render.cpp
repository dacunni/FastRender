#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>

#include <sys/stat.h>
#include <stdio.h>
#include <math.h>

#include "AxisAlignedSlab.h"
#include "Matrix.h"
#include "Transform.h"
#include "Ray.h"
#include "Sphere.h"
#include "RandomNumberGenerator.h"
#include "Timer.h"
#include "Plot2D.h"
#include "SimpleCamera.h"
#include "ImageTracer.h"
#include "TestScenes.h"
#include "Container.h"
#include "FlatContainer.h"
#include "AmbientOcclusionShader.h"
#include "BasicDiffuseSpecularShader.h"
#include "Material.h"

RandomNumberGenerator rng;
std::string output_path = "testoutput";
unsigned int plot_size = 500;

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
// Ambient occlusion
// ------------------------------------------------------------ 
// White spheres
void testAO1()
{
    int imageSize = 320;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    container->add( new Sphere( -2, 0.25, 0, 0.25 ) );
    container->add( new Sphere( -1, 0.50, 0, 0.50 ) );
    container->add( new Sphere( +1, 1.00, 0, 1.00 ) );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new AmbientOcclusionShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_ao1_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.render();
}

// White spheres
void testAO2()
{
    int imageSize = 320;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    container->add( new Sphere( -2, 0.25, 0, 0.25 ) );
    container->add( new Sphere( -1, 0.50, 0, 0.50 ) );
    container->add( new Sphere( +1, 1.00, 0, 1.00 ) );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new AmbientOcclusionShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_ao2_";

    // Camera above looking down
    Transform rotation = makeRotation( -M_PI / 2, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.render();
}

// White cubes
void testAO3()
{
    int imageSize = 320;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    container->add( new AxisAlignedSlab( +1.75, 0, 0, 1.25 ) );
    container->add( new AxisAlignedSlab( +0.0, 0, 0, 1.00 ) );
    container->add( new AxisAlignedSlab( -1.5, 0, 0, 0.75 ) );
    container->add( new AxisAlignedSlab( -2.5, 0, 0, 0.50 ) );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new AmbientOcclusionShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_ao3_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.render();
}

// Colored cubes
void testAO4()
{
    int imageSize = 320;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    AxisAlignedSlab * cube = nullptr;

    cube = new AxisAlignedSlab( +1.75, 0, 0, 1.25 );
    cube->material = new DiffuseMaterial( 1.0, 1.0, 0.5 );
    container->add( cube );

    cube = new AxisAlignedSlab( +0.0, 0, 0, 1.00 );
    cube->material = new DiffuseMaterial( 1.0, 0.5, 0.5 );
    container->add( cube );

    cube = new AxisAlignedSlab( -1.5, 0, 0, 0.75 );
    cube->material = new DiffuseMaterial( 0.5, 0.5, 1.0 );
    container->add( cube );

    cube = new AxisAlignedSlab( -2.5, 0, 0, 0.50 );
    cube->material = new DiffuseMaterial( 0.5, 1.0, 0.5 );
    container->add( cube );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new AmbientOcclusionShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_ao4_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.render();
}


// ------------------------------------------------------------ 
// Test runner
// ------------------------------------------------------------ 
int main (int argc, char * const argv[]) 
{
    printf("Render Tests\n");
    fflush(stdout);

    mkdir(output_path.c_str(), 0777);

    Timer total_run_timer;
    total_run_timer.start();

    rng.seedCurrentTime();

    // Tests
    testRayIntersect();
    testSimpleCameraNoJitter();
    testSimpleCamera();
    testAO1();
    testAO2();
    testAO3();
    testAO4();
    
    total_run_timer.stop();
    printf("Done - Run time = %f seconds\n", total_run_timer.elapsed());
    fflush(stdout);
    return 0;
}

