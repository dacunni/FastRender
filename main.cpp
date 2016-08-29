#include <iostream>
#include <fstream>
#include <stdio.h>

// TEMP >>>
//#include <Magick++.h>
#include <math.h>
// TEMP <<<

#include "Artifacts.h"
#include "Matrix.h"
#include "Transform.h"
#include "Ray.h"
#include "Sphere.h"
#include "RandomNumberGenerator.h"
#include "Scene.h"
#include "Shader.h"
#include "SimpleCamera.h"
#include "AmbientOcclusionShader.h"
#include "BasicDiffuseSpecularShader.h"
#include "FlatContainer.h"
#include "AxisAlignedSlab.h"
#include "TriangleMesh.h"
#include "Timer.h"
#include "BoundingVolume.h"
#include "TMOctreeAccelerator.h"
#include "Material.h"
#include "TestScenes.h"
#include "EnvironmentMap.h"
#include "ImageTracer.h"
#include "Quaternion.h"
#include "Types.h"

RandomNumberGenerator rng;

Scene * buildScene()
{
    Scene * scene = new Scene();

    printf( "Building scene\n" );
    Timer build_scene_timer;
    build_scene_timer.start();
	FlatContainer * container = new FlatContainer();
	
    //addLightingTest2( container );
    //addSlabGrid( container );
    addOffsetCubes( container );
    //addLitBunny( container );
    addBunny( container );
    //addRandomSpheres( container, rng, 20 );
    addGroundPlane( container );
    //addTransformedCubes( container );
    //addLightingTest4( container );

    //Sphere * sphere = new Sphere( Vector4( 0.0, 1.0, -3.0 ), 1.0 );
    //Sphere * sphere = new Sphere( Vector4( 0.0, 2.0, -5.0 ), 1.0 );
    Sphere * sphere = new Sphere( Vector4( -3.0, 1.0, -7.0 ), 1.0 );
    sphere->material = new MirrorMaterial();
    container->add( sphere );

    Sphere * sphere2 = new Sphere( Vector4( -1.0, 0.1, -0.0 ), 0.25 );
    sphere2->material = new DiffuseMaterial( 1.0, 0.0, 1.0 );
    container->add( sphere2 );

    // Tetrahedron for quick testing of mesh reflection bug
    //TriangleMesh * mesh = new TriangleMesh();
    //makeTriangleMeshTetrahedron( *mesh );
    //mesh->transform = new Transform();
    //*mesh->transform = makeTranslation( Vector4( 0.0, 0.3, 0.0 ) );
    //container->add( mesh );

    // Cubes for reference
    AxisAlignedSlab * cube1 = new AxisAlignedSlab( 1.0, -0.5, -3.0, 0.5 );
    cube1->material = new DiffuseMaterial( 0.0, 0.0, 1.0 );
    cube1->transform = new Transform();
    *cube1->transform = makeTranslation( Vector4( 0.1, 0.5, -0.5 ) );
    container->add( cube1 );
    AxisAlignedSlab * cube2 = new AxisAlignedSlab( 1.0, -0.5+1.0, -3.0, 0.5 );
    cube2->material = new DiffuseMaterial( 0.0, 1.0, 0.0 );
    container->add( cube2 );

    //addMirrors( container );

	scene->root = container;
    //scene->env_map = new TestPatternEnvironmentMap();
    scene->env_map = new ArcLightEnvironmentMap();
    //scene->env_map = new ArcLightEnvironmentMap( Vector4( 0.0, 1.0, 1.0 ), M_PI / 4.0 );
    //scene->env_map = new ArcLightEnvironmentMap( Vector4( 0.0, 1.0, 1.0 ), M_PI / 8.0 );
    scene->buildLightList();
    build_scene_timer.stop();
    printf( "Build scene: %f seconds\n", build_scene_timer.elapsed() );

    return scene;
}

void testScene()
{
    //int imageSize = 50;
    int imageSize = 320;
    //int imageSize = 512;
    //int imageSize = 1024;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight );
    tracer.scene = buildScene();

    //tracer.shader = new AmbientOcclusionShader();
    tracer.shader = new BasicDiffuseSpecularShader();

    //tracer.artifacts.file_prefix = "test_";

#if 0
    tracer.setCameraTransform( makeTranslation( 0.0, 0.0, 5.0 ) );
#elif 1
    Transform rotation = compose( makeRotation( M_PI / 4, Vector4(0, 1, 0) ),
                                  makeRotation( -0.2, Vector4(1, 0, 0) ) );
    Transform translation = makeTranslation( 2.0, 2.0, 25.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );
#else
    auto xform_lambda = [](float anim_progress) {
        Vector4 rot_axis( 0.0, 1.0, 0.0 ); rot_axis.normalize();
        float min_angle = -0.0, max_angle = M_PI / 8;
        float angle = (anim_progress * (max_angle - min_angle)) + min_angle;
        Transform rotation = makeRotation( angle, rot_axis );
        Vector4 begin_xlate( 0.0, 0.5, 10.0 ), end_xlate( 0.0, 0.5, 15.0 );
        //Vector4 begin_xlate( 1.5, 0.25, 15.0 ), end_xlate( 2.0, 0.25, 7.0 );
        //Vector4 begin_xlate( 1.5, 1.5, 5.0 ), end_xlate( 2.0, 0.25, 5.0 );
        Vector4 xlate = interp( begin_xlate, end_xlate, anim_progress);
        Transform translation = makeTranslation( xlate );
        return compose( rotation, translation );
    };
    tracer.setCameraTransform( xform_lambda );
    tracer.num_frames = 20;
#endif

    tracer.render();
}

// TODO - make tests for Transforms

int main (int argc, char * const argv[]) 
{
    printf("FastRender\n");
    fflush(stdout);
    Timer total_run_timer;
    total_run_timer.start();

    rng.seedCurrentTime();

    testScene();
    
    total_run_timer.stop();
    printf("Done - Run time = %f seconds\n", total_run_timer.elapsed());
    fflush(stdout);
    return 0;
}

