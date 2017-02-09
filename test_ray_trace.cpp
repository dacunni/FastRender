#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>
#include <functional>

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
        plot.strokeColor( 0, 0, 0 );
        plot.drawLine( last_cos1, last_f, cos1, f );
        last_cos1 = cos1;
        last_f = f;
    }
    last_cos1 = 0.0;
    last_f = fresnelDialectricParallelSnell( 0.0, n1, n2 );
    for( float cos1 = 0.0; cos1 <= 1.0; cos1 += dc ) {
        float f = fresnelDialectricParallelSnell( cos1, n1, n2 );
        plot.strokeColor( 0, 0, 1 );
        plot.drawLine( last_cos1, last_f, cos1, f );
        last_cos1 = cos1;
        last_f = f;
    }
    last_cos1 = 0.0;
    last_f = fresnelDialectricPerpendicularSnell( 0.0, n1, n2 );
    for( float cos1 = 0.0; cos1 <= 1.0; cos1 += dc ) {
        float f = fresnelDialectricPerpendicularSnell( cos1, n1, n2 );
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
        plot.strokeColor( 0, 0, 0 );
        plot.drawLine( last_cos1, last_f, cos1, f );
        last_cos1 = cos1;
        last_f = f;
    }
    last_cos1 = 0.0;
    last_f = fresnelDialectricParallelSnell( 0.0, n1, n2 );
    for( float cos1 = 0.0; cos1 <= 1.0; cos1 += dc ) {
        float f = fresnelDialectricParallelSnell( cos1, n1, n2 );
        plot.strokeColor( 0, 0, 1 );
        plot.drawLine( last_cos1, last_f, cos1, f );
        last_cos1 = cos1;
        last_f = f;
    }
    last_cos1 = 0.0;
    last_f = fresnelDialectricPerpendicularSnell( 0.0, n1, n2 );
    for( float cos1 = 0.0; cos1 <= 1.0; cos1 += dc ) {
        float f = fresnelDialectricPerpendicularSnell( cos1, n1, n2 );
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


enum LightSourcePattern { PATTERN_RADIAL, PATTERN_PARALLEL };
void testRefractObject(std::shared_ptr<Traceable> obj,
                       const std::string & plot_file_name,
                       std::function<void(Plot2D&)> plot_obj,
                       LightSourcePattern light_pattern)
{
    Plot2D plot( output_path + "/" + plot_file_name, 1000, 1000,
                 -2.0, 4.0, -3.0, 3.0 );
    Scene scene;
    Vector4 o( -1, 1, 0 ), d;
    auto at = Vector4( 0.0, 0.0, 0.0 );

    plot.fillColor( 0.8, 0.8, 1.0 );
    plot.strokeColor( 0.4, 0.4, 1.0 );
    plot_obj( plot );

    obj->material = std::make_shared<RefractiveMaterial>(N_DIAMOND);
    scene.root = obj;

    TraceLog log; scene.startLogging( &log );
    BasicDiffuseSpecularShader shader;

    float radius = 1.5;
    int nsteps = 35;
    float alpha = 0.0;

    for( int i = 1; i <= nsteps; i++ ) {
        log.entries.clear();

        if( light_pattern == PATTERN_RADIAL ) {
            alpha = (float) i / (nsteps + 2);
            float angle = (1.0 - alpha) * M_PI + alpha * M_PI / 2.0;
            o.x = radius * cos(angle);
            o.y = radius * sin(angle);
            d = subtract( at, o ).normalized();
        }
        else if( light_pattern == PATTERN_PARALLEL ) {
            alpha = (float) i / nsteps;
            o = blend( Vector4( -1.5, 0, 0 ), 1.0 - alpha, Vector4( 0, 1.5, 0 ), alpha );
            d = Vector4( 1, -1, 0 ).normalized();
        }

        Ray ray( o, d );
        RayIntersection intersection;

        bool hit = scene.intersect( ray, intersection );

        if( hit ) {
            if( intersection.distance != FLT_MAX ) {
                shader.shade( scene, rng, intersection );
            }
        }

        for( int i = 0; i < (int) log.entries.size(); i++ ) {
            auto & entry = log.entries[i];
            auto & o = entry.ray.origin;
            auto & d = entry.ray.direction;
            plot.strokeColor( alpha, 0.0, 1.0 - alpha );
            plot.fillColor( alpha, 0.0, 1.0 - alpha );
            plot.drawCircle( o.x, o.y, 0.03 );
            plot.drawLine( o, add( o, scale( d, 0.4 ) ) );
        }
    }
}

void testRefractAxisAlignedSlab()
{
    auto obj = std::make_shared<AxisAlignedSlab>( -1.0, -1.5, -2.0,
                                                   3.0,  0.0,  2.0 );
    auto plot_obj = [obj](Plot2D & plot) {
        plot.drawRect( obj->xmin, obj->ymin, obj->xmax, obj->ymax );
    };

    testRefractObject( std::dynamic_pointer_cast<Traceable>(obj),
                       std::string("refract_slab_parallel.png"), plot_obj, PATTERN_PARALLEL );
    testRefractObject( std::dynamic_pointer_cast<Traceable>(obj),
                       std::string("refract_slab_radial.png"), plot_obj, PATTERN_RADIAL );
}

void testRefractSphere()
{
    auto obj = std::make_shared<Sphere>( 1.0, -1.0, 0.0, 1.0 );

    auto plot_obj = [obj](Plot2D & plot) {
        plot.drawCircle( obj->center.x, obj->center.y, obj->radius );
    };

    testRefractObject( std::dynamic_pointer_cast<Traceable>(obj),
                       std::string("refract_sphere_radial.png"), plot_obj, PATTERN_RADIAL );
    testRefractObject( std::dynamic_pointer_cast<Traceable>(obj),
                       std::string("refract_sphere_parallel.png"), plot_obj, PATTERN_PARALLEL );
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
// Timing
// ------------------------------------------------------------ 
void testRayObjectTiming(Traceable & traceable, const char * name, bool test_any = false)
{
    std::vector<Ray> rays;
    int ray_pool_size = 1000;
    int num_rays = 100000000;
    //int num_rays = 10000;
    double timer_cutoff = 5.0;
    
    for( int i = 0; i < ray_pool_size; i++ ) {
        Ray ray;
        float x = rng.uniformRange( -1.0, 1.0 );
        float y = rng.uniformRange( -1.0, 1.0 );
        ray.origin = Vector4( x, y, -10 );
        ray.direction = Vector4( 0, 0, 1, 0 );
        rays.push_back( ray );
    }

    RayIntersection isect;
    bool hit;

    Timer timer;
    timer.start();
    for( int i = 0; i < num_rays; i++ ) {
        int j = i % ray_pool_size;
        if( test_any ) {
            hit = traceable.intersectsAny( rays[j], EPSILON );
        }
        else {
            hit = traceable.intersect( rays[j], isect );
        }
        if( i % 1000 == 0
            && timer.elapsed() > timer_cutoff ) {
            num_rays = i + 1;
            break;
        }
    }
    timer.stop();
    float elapsed = timer.elapsed();
    float rays_per_second = (float) num_rays / elapsed;
    printf("%s (any=%d) : %d rays in %f seconds = %.2f rays / sec\n",
           name, (int)test_any, num_rays, elapsed, rays_per_second);
}

void testRaySphereTiming()
{
    Sphere sphere( 0, 0, 0, 1 );
    testRayObjectTiming( sphere, "Sphere", false );
    testRayObjectTiming( sphere, "Sphere", true );
}

void testRayAxisAlignedSlabTiming()
{
    AxisAlignedSlab slab( -1, -1, -1, 1, 1, 1 );
    testRayObjectTiming( slab, "AxisAlignedSlab", false );
    testRayObjectTiming( slab, "AxisAlignedSlab", true );
}

void testRayMeshBunnyTiming()
{
    AssetLoader loader;
    std::string modelBasePath = "models";
    std::string modelPath = modelBasePath + "/stanford/bunny/reconstruction";
    auto mesh = loader.load( modelPath + "/bun_zipper.ply", false );
    mesh->makeCanonical();
    testRayObjectTiming( *mesh, "MeshBunny", false );
    testRayObjectTiming( *mesh, "MeshBunny", true );
}

void testRayMeshOctreeBunnyTiming()
{
    AssetLoader loader;
    std::string modelBasePath = "models";
    std::string modelPath = modelBasePath + "/stanford/bunny/reconstruction";
    auto mesh = loader.load( modelPath + "/bun_zipper.ply", true );
    mesh->makeCanonical();
    TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *std::dynamic_pointer_cast<TriangleMesh>(mesh) );
    mesh_octree->build();
    mesh->accelerator = mesh_octree;
    testRayObjectTiming( *mesh, "MeshOctreeBunny", false );
    testRayObjectTiming( *mesh, "MeshOctreeBunny", true );
}

void testVectorTiming()
{
    std::vector<Vector4> vectors;
    unsigned long num_iterations = 100000000;
    unsigned int pool_size = 1000;

    // Setup some vectors for the timing code to use
    for( int i = 0; i < pool_size; i++ ) {
        Vector4 v;
        rng.uniformSurfaceUnitSphere( v );
        vectors.push_back( v );
    }

    std::map<std::string, float> results;
    Timer timer;

    // Member functions
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        vectors[i % pool_size].z = vectors[(i+1) % pool_size][i % 4];
    } timer.stop(); results["u[i] -> w"] = timer.elapsed();
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        vectors[(i+1) % pool_size][i % 4] = vectors[i % pool_size].z;
    } timer.stop(); results["u[i] <- w"] = timer.elapsed();
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        vectors[i % pool_size].w = vectors[(i+1) % pool_size].magnitude_sq();
    } timer.stop(); results["u[i].magnitude_sq()"] = timer.elapsed();
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        vectors[i % pool_size].w = vectors[(i+1) % pool_size].magnitude();
    } timer.stop(); results["u[i].magnitude()"] = timer.elapsed();
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        vectors[i % pool_size] = vectors[(i+1) % pool_size].normalized();
    } timer.stop(); results["u[i].normalized() -> w"] = timer.elapsed();
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        vectors[(i+1) % pool_size].normalize();
    } timer.stop(); results["u[i].normalize()"] = timer.elapsed();
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        vectors[i % pool_size] = vectors[(i+1) % pool_size].negated();
    } timer.stop(); results["u[i].negated() -> w"] = timer.elapsed();
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        vectors[(i+1) % pool_size].negate();
    } timer.stop(); results["u[i].negate()"] = timer.elapsed();

    // Functions between vectors
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        vectors[i % pool_size] = add( vectors[(i+1) % pool_size], vectors[(i+2) % pool_size] );
    } timer.stop(); results["add(u, v) -> w"] = timer.elapsed();
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        add( vectors[(i+1) % pool_size], vectors[(i+2) % pool_size], vectors[i % pool_size] );
    } timer.stop(); results["add(u, v, w)"] = timer.elapsed();

    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        vectors[i % pool_size] = subtract( vectors[(i+1) % pool_size], vectors[(i+2) % pool_size] );
    } timer.stop(); results["subtract(u, v) -> w"] = timer.elapsed();
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        subtract( vectors[(i+1) % pool_size], vectors[(i+2) % pool_size], vectors[i % pool_size] );
    } timer.stop(); results["subtract(u, v, w)"] = timer.elapsed();

    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        vectors[i % pool_size].x = dot( vectors[(i+1) % pool_size], vectors[(i+2) % pool_size] );
    } timer.stop(); results["dot(u, v) -> w"] = timer.elapsed();
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        dot( vectors[(i+1) % pool_size], vectors[(i+2) % pool_size], vectors[i % pool_size].x );
    } timer.stop(); results["dot(u, v, w)"] = timer.elapsed();

    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        vectors[i % pool_size] = cross( vectors[(i+1) % pool_size], vectors[(i+2) % pool_size] );
    } timer.stop(); results["cross(u, v) -> w"] = timer.elapsed();
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        cross( vectors[(i+1) % pool_size], vectors[(i+2) % pool_size], vectors[i % pool_size] );
    } timer.stop(); results["cross(u, v, w)"] = timer.elapsed();

    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        vectors[i % pool_size] = scale( vectors[(i+1) % pool_size], vectors[(i+2) % pool_size].x );
    } timer.stop(); results["scale(u, v) -> w"] = timer.elapsed();
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        scale( vectors[(i+1) % pool_size], vectors[(i+2) % pool_size].x, vectors[i % pool_size] );
    } timer.stop(); results["scale(u, v, w)"] = timer.elapsed();

    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        vectors[i % pool_size] = blend( vectors[(i+1) % pool_size], vectors[(i+1) % pool_size].x,
                                        vectors[(i+2) % pool_size], vectors[(i+1) % pool_size].y );
    } timer.stop(); results["blend(u, s, v, t) -> w"] = timer.elapsed();
    
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        vectors[i % pool_size] = mirror( vectors[(i+1) % pool_size], vectors[(i+2) % pool_size] );
    } timer.stop(); results["mirror(u, v) -> w"] = timer.elapsed();
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        mirror( vectors[(i+1) % pool_size], vectors[(i+2) % pool_size], vectors[i % pool_size] );
    } timer.stop(); results["mirror(u, v, w)"] = timer.elapsed();

    float n1 = 1.03, n2 = 1.4;
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        vectors[i % pool_size] = refract( vectors[(i+1) % pool_size], vectors[(i+2) % pool_size], n1, n2 );
    } timer.stop(); results["refract(u, v, n1, n2) -> w"] = timer.elapsed();
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        refract( vectors[(i+1) % pool_size], vectors[(i+2) % pool_size], n1, n2, vectors[i % pool_size] );
    } timer.stop(); results["refract(u, v, n1, n2, w)"] = timer.elapsed();

    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        vectors[i % pool_size] = interp( vectors[(i+1) % pool_size], vectors[(i+2) % pool_size], 0.25 );
    } timer.stop(); results["interp(u, v, a) -> w"] = timer.elapsed();
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        interp( vectors[(i+1) % pool_size], vectors[(i+2) % pool_size], 0.25, vectors[i % pool_size] );
    } timer.stop(); results["interp(u, v, a, w)"] = timer.elapsed();

    // Results
    printf("Vector Operations Timing:\n");
    for( auto result : results ) {
        printf("%30s : %lu ops in %6.2f seconds = %15.0f ops / sec\n",
               result.first.c_str(), num_iterations, result.second, (float) num_iterations / result.second);

    }
}

void testMatrixTiming()
{
    std::vector<Matrix4x4> matrices;
    std::vector<Vector4> vectors;
    unsigned long num_iterations = 100000000;
    unsigned int pool_size = 1000;

    // Setup some matrices and vectors for the timing code to use
    for( int i = 0; i < pool_size; i++ ) {
        Matrix4x4 m;
        for( int j = 0; j < 16; j++ ) {
            m.data[j] = rng.uniformRange( -10.0, 10.0 );
        }
        matrices.push_back( m );
        Vector4 v;
        rng.uniformSurfaceUnitSphere( v );
        vectors.push_back( v );
    }

    std::map<std::string, float> results;
    Timer timer;

    // Functions between matrices and vectors
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        matrices[i % pool_size] = mult( matrices[(i+1) % pool_size], matrices[(i+2) % pool_size] );
    } timer.stop(); results["mult(M, N) -> P"] = timer.elapsed();
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        mult( matrices[(i+1) % pool_size], matrices[(i+2) % pool_size], matrices[i % pool_size] );
    } timer.stop(); results["mult(M, N, P)"] = timer.elapsed();

    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        vectors[i % pool_size] = mult( matrices[(i+1) % pool_size], vectors[(i+2) % pool_size] );
    } timer.stop(); results["mult(M, u) -> v"] = timer.elapsed();
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        mult( matrices[(i+1) % pool_size], vectors[(i+2) % pool_size], vectors[i % pool_size] );
    } timer.stop(); results["mult(M, u, v)"] = timer.elapsed();

    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        matrices[i % pool_size] = inverse( matrices[(i+1) % pool_size] );
    } timer.stop(); results["inverse(M) -> P"] = timer.elapsed();
    timer.start(); for( unsigned long i = 0; i < num_iterations; ++i ) {
        inverse( matrices[(i+1) % pool_size], matrices[(i+2) % pool_size] );
    } timer.stop(); results["inverse(M, P)"] = timer.elapsed();

    // Results
    printf("Matrix Operations Timing:\n");
    for( auto result : results ) {
        printf("%30s : %lu ops in %6.2f seconds = %15.0f ops / sec\n",
               result.first.c_str(), num_iterations, result.second, (float) num_iterations / result.second);

    }
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
#if 0
    testRayIntersect();
    testReflectAngles();
    testSnellAngles();
    testRefractAnglesLowHigh();
    testRefractAnglesHighLow();
    testRefractAnglesEqualIndex();
    testRefractAxisAlignedSlab();
    testRefractSphere();
    testFresnelDialectric1();
    testFresnelDialectric2();
    testFresnelDialectric3();
    testFresnelDialectric4();
    testSimpleCameraNoJitter();
    testSimpleCamera();
    // Timing
    testVectorTiming();
    testMatrixTiming();
    testRaySphereTiming();
    testRayAxisAlignedSlabTiming();
    testRayMeshBunnyTiming();
    testRayMeshOctreeBunnyTiming();
#else
    //testVectorTiming();
    //testMatrixTiming();
    //testRaySphereTiming();
    //testRayAxisAlignedSlabTiming();
    testRayMeshBunnyTiming();
    testRayMeshOctreeBunnyTiming();
#endif
    
    total_run_timer.stop();
    printf("Done - Run time = %f seconds\n", total_run_timer.elapsed());
    fflush(stdout);
    return 0;
}

