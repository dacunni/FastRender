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

// Triangle mesh - Stanford bunny
void testAO5()
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

    AssetLoader loader;
    std::string modelBasePath = "models";
    std::string modelPath = modelBasePath + "/stanford/bunny/reconstruction";
    TriangleMesh * mesh = loader.load( modelPath + "/bun_zipper_res2.ply" );
    AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();

    if( !mesh ) {
        fprintf( stderr, "Error loading mesh\n" );
        return;
    }

    mesh->material = new DiffuseMaterial( 1.0, 1.0, 1.0 );

    TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
    mesh_octree->build();
    mesh->accelerator = mesh_octree;
    mesh->transform = new Transform();
    *mesh->transform = compose( makeScaling( 2, 2, 2 ),
                                makeTranslation( Vector4( 0.0, -bounds->ymin, 1.0 ) ) );
    container->add( mesh );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new AmbientOcclusionShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_ao5_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.render();
}

// Triangle mesh - Stanford dragon
void testAO6()
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

    AssetLoader loader;
    std::string modelBasePath = "models";
    std::string modelPath = modelBasePath + "/stanford/dragon/reconstruction";
    TriangleMesh * mesh = loader.load( modelPath + "/dragon_vrip_res2.ply" );
    AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();

    if( !mesh ) {
        fprintf( stderr, "Error loading mesh\n" );
        return;
    }

    mesh->material = new DiffuseMaterial( 1.0, 1.0, 1.0 );

    TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
    mesh_octree->build();
    mesh->accelerator = mesh_octree;
    mesh->transform = new Transform();
    *mesh->transform = compose( makeScaling( 2, 2, 2 ),
                                makeTranslation( Vector4( 0.0, -bounds->ymin, 1.0 ) ) );
    container->add( mesh );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new AmbientOcclusionShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_ao6_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 10.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.render();
}

// Triangle mesh - Stanford Happy Buddha
void testAO7()
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

    AssetLoader loader;
    std::string modelBasePath = "models";
    std::string modelPath = modelBasePath + "/stanford/happy/reconstruction";
    TriangleMesh * mesh = loader.load( modelPath + "/happy_vrip_res2.ply" );
    AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();

    if( !mesh ) {
        fprintf( stderr, "Error loading mesh\n" );
        return;
    }

    mesh->material = new DiffuseMaterial( 1.0, 1.0, 1.0 );

    TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
    mesh_octree->build();
    mesh->accelerator = mesh_octree;
    mesh->transform = new Transform();
    *mesh->transform = compose( makeScaling( 4, 4, 4 ),
                                makeTranslation( Vector4( 0.0, -bounds->ymin, 0.5 ) ) );
    container->add( mesh );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new AmbientOcclusionShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_ao7_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 4, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.render();
}

// ------------------------------------------------------------ 
// Lighting
// ------------------------------------------------------------ 
// White spheres
void testSphereLight1()
{
    int imageSize = 512;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 40 );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    container->add( new Sphere( -2, 0.25, 0, 0.25 ) );
    container->add( new Sphere( -1, 0.50, 0, 0.50 ) );
    container->add( new Sphere( +1, 1.00, 0, 1.00 ) );

    addSphereLight( container,
                    Vector4( 5.0, 5.0, 5.0 ), 2.5,
                    RGBColor( 1.0, 1.0, 1.0 ), 15.0 );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_sphere_light1_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

// Colored light
void testSphereLight2()
{
    int imageSize = 512;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 40 );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    container->add( new Sphere( -2, 0.25, 0, 0.25 ) );
    container->add( new Sphere( -1, 0.50, 0, 0.50 ) );
    container->add( new Sphere( +1, 1.00, 0, 1.00 ) );

    addSphereLight( container,
                    Vector4( 5.0, 5.0, 5.0 ), 2.5,
                    RGBColor( 0.5, 0.5, 1.0 ), 15.0 );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_sphere_light2_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

// Colored objects
void testSphereLight3()
{
    int imageSize = 512;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 40 );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    Sphere * sphere = nullptr;

    sphere = new Sphere( -2, 0.25, 0, 0.25 );
    sphere->material = new DiffuseMaterial( 1.0, 0.5, 0.5 );
    container->add( sphere );

    sphere = new Sphere( -1, 0.50, 0, 0.50 );
    sphere->material = new DiffuseMaterial( 0.5, 1.0, 0.5 );
    container->add( sphere );

    sphere = new Sphere( +1, 1.00, 0, 1.00 );
    sphere->material = new DiffuseMaterial( 0.5, 0.5, 1.0 );
    container->add( sphere );

    addSphereLight( container,
                    Vector4( 5.0, 5.0, 5.0 ), 2.5,
                    RGBColor( 1.0, 1.0, 1.0 ), 15.0 );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_sphere_light3_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

// Multiple colored lights
void testSphereLight4()
{
    int imageSize = 512;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 40 );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    container->add( new Sphere( -2, 0.25, 0, 0.25 ) );
    container->add( new Sphere( -1, 0.50, 0, 0.50 ) );
    container->add( new Sphere( +1, 1.00, 0, 1.00 ) );

    float light_size = 1.5;
    float light_power = 25.0;
    addSphereLight( container,
                    Vector4( 5.0, 5.0, 5.0 ), light_size,
                    RGBColor( 0.5, 0.5, 1.0 ), light_power );
    addSphereLight( container,
                    Vector4( -5.0, 5.0, 5.0 ), light_size,
                    RGBColor( 1.0, 0.5, 0.5 ), light_power );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_sphere_light4_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

// White point light
void testPointLight1()
{
    int imageSize = 512;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 40 );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    container->add( new Sphere( -2, 0.25, 0, 0.25 ) );
    container->add( new Sphere( -1, 0.50, 0, 0.50 ) );
    container->add( new Sphere( +1, 1.00, 0, 1.00 ) );

    scene->addPointLight( PointLight( Vector4( -5.0, 5.0, 5.0 ),
        RGBColor( 1.0, 1.0, 1.0 ).scaled(20.0) ) );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_point_light1_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

// Multiple colored point lights
void testPointLight2()
{
    int imageSize = 512;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 40 );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    container->add( new Sphere( -2, 0.25, 0, 0.25 ) );
    container->add( new Sphere( -1, 0.50, 0, 0.50 ) );
    container->add( new Sphere( +1, 1.00, 0, 1.00 ) );

    scene->addPointLight( PointLight( Vector4( -5.0, 5.0, 5.0 ),
        RGBColor( 1.0, 1.0, 1.0 ).scaled(20.0) ) );
    scene->addPointLight( PointLight( Vector4( 3.0, 5.0, 10.0 ),
        RGBColor( 1.0, 0.4, 0.4 ).scaled(20.0) ) );
    scene->addPointLight( PointLight( Vector4( 0.0, 10.0, 5.0 ),
        RGBColor( 0.4, 0.4, 1.0 ).scaled(20.0) ) );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_point_light2_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

void testPointLight3()
{
    int imageSize = 512;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 40 );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    AssetLoader loader;
    std::string modelBasePath = "models";
    std::string modelPath = modelBasePath + "/stanford/bunny/reconstruction";
    TriangleMesh * mesh = loader.load( modelPath + "/bun_zipper_res2.ply" );

    if( !mesh ) {
        fprintf( stderr, "Error loading mesh\n" );
        return;
    }

    AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();

    //mesh->material = new DiffuseMaterial( 0.75, 1.0, 0.8 );

    TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
    mesh_octree->build();
    mesh->accelerator = mesh_octree;
    mesh->transform = new Transform();
    *mesh->transform = compose( makeScaling( 2, 2, 2 ),
                                makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
    container->add( mesh );

    container->add( new Sphere( -2, 0.25, 0, 0.25 ) );

    auto cube = new AxisAlignedSlab( 0.8, 0, 2.0, 0.5 );
    cube->material = new DiffuseMaterial( 1.0, 0.2, 0.2 );
    container->add( cube );

    scene->addPointLight( PointLight( Vector4( -15.0, 15.0, 15.0 ),
        RGBColor( 1.0, 1.0, 1.0 ).scaled(150.0) ) );
    scene->addPointLight( PointLight( Vector4( 13.0, 15.0, 20.0 ),
        RGBColor( 1.0, 0.4, 0.4 ).scaled(150.0) ) );
    scene->addPointLight( PointLight( Vector4( 0.0, 20.0, 15.0 ),
        RGBColor( 0.4, 0.4, 1.0 ).scaled(150.0) ) );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_point_light3_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

// Mirror Bunny and friends
void testPointLight4()
{
    int imageSize = 512;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 40 );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    AssetLoader loader;
    std::string modelBasePath = "models";
    std::string modelPath = modelBasePath + "/stanford/bunny/reconstruction";
    TriangleMesh * mesh = loader.load( modelPath + "/bun_zipper_res2.ply" );

    if( !mesh ) {
        fprintf( stderr, "Error loading mesh\n" );
        return;
    }

    AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();

    mesh->material = new MirrorMaterial();

    TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
    mesh_octree->build();
    mesh->accelerator = mesh_octree;
    mesh->transform = new Transform();
    *mesh->transform = compose( makeScaling( 2, 2, 2 ),
                                makeRotation( 0.25 * M_PI, Vector4(0, 1, 0) ),
                                makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
    container->add( mesh );

    container->add( new Sphere( -2, 0.25, 0, 0.25 ) );

    // colored balls to see if we are gettingn proper reflections
    auto s = new Sphere( -1.25, 0.25, 0.75, 0.25 );
    s->material = new DiffuseMaterial(1, 1, 0);
    container->add( s );
    s = new Sphere( 1.25, 0.25, 0.75, 0.25 );
    s->material = new DiffuseMaterial(0, 1, 0);
    container->add( s );
    s = new Sphere( 0.0, 0.25, 3.0, 0.25 );
    s->material = new DiffuseMaterial(0, 1, 1);
    container->add( s );

    auto cube = new AxisAlignedSlab( 0.8, 0, 2.0, 0.5 );
    cube->material = new DiffuseMaterial( 1.0, 0.2, 0.2 );
    container->add( cube );

    scene->addPointLight( PointLight( Vector4( -15.0, 15.0, 15.0 ),
        RGBColor( 1.0, 1.0, 1.0 ).scaled(150.0) ) );
    scene->addPointLight( PointLight( Vector4( 13.0, 15.0, 20.0 ),
        RGBColor( 1.0, 0.4, 0.4 ).scaled(150.0) ) );
    scene->addPointLight( PointLight( Vector4( 0.0, 20.0, 15.0 ),
        RGBColor( 0.4, 0.4, 1.0 ).scaled(150.0) ) );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_point_light4_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

// ------------------------------------------------------------ 
// Reflection
// ------------------------------------------------------------ 
void testReflection1()
{
    int imageSize = 320;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 100 );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    Sphere * sphere = nullptr;

    sphere = new Sphere( -2, 0.25, 0.5, 0.25 );
    sphere->material = new DiffuseMaterial( 1.0, 0.5, 0.5 );
    container->add( sphere );

    sphere = new Sphere( 0, 0.50, 1.5, 0.50 );
    sphere->material = new DiffuseMaterial( 0.5, 1.0, 0.5 );
    container->add( sphere );

    sphere = new Sphere( -1, 0.75, 0, 0.75  );
    sphere->material = new MirrorMaterial();
    container->add( sphere );

    sphere = new Sphere( +1, 1.00, 0, 1.00 );
    sphere->material = new DiffuseMaterial( 0.5, 0.5, 1.0 );
    container->add( sphere );

	scene->root = container;
    scene->env_map = new ArcLightEnvironmentMap();
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_reflect1_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -0.2, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

void testReflection2()
{
    int imageSize = 320;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 100 );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    Sphere * sphere = nullptr;

    sphere = new Sphere( -2, 0.25, 0.5, 0.25 );
    sphere->material = new DiffuseMaterial( 1.0, 0.5, 0.5 );
    container->add( sphere );

    sphere = new Sphere( +1, 0.50, 4.0, 0.50 );
    sphere->material = new DiffuseMaterial( 0.5, 1.0, 0.5 );
    container->add( sphere );

    sphere = new Sphere( -1, 0.75, 0, 0.75  );
    sphere->material = new MirrorMaterial( 0.75, 0.75, 0.15 );
    container->add( sphere );

    sphere = new Sphere( +1, 1.00, 0, 1.00 );
    sphere->material = new DiffuseMaterial( 0.5, 0.5, 1.0 );
    container->add( sphere );

    sphere = new Sphere( 0, 0.5, 2.0, 0.5  );
    sphere->material = new MirrorMaterial();
    container->add( sphere );

    sphere = new Sphere( -0.75, 0.25, 3.5, 0.25  );
    sphere->material = new MirrorMaterial( 1.0, 0.0, 1.0 );
    container->add( sphere );

	scene->root = container;
    scene->env_map = new ArcLightEnvironmentMap();
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_reflect2_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -0.2, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

void testReflection3()
{
    int imageSize = 320;
    //int imageSize = 512;
    //int imageSize = 1024;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 100 );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    AssetLoader loader;
    std::string modelBasePath = "models";
    std::string modelPath = modelBasePath + "/stanford/bunny/reconstruction";
    TriangleMesh * mesh = loader.load( modelPath + "/bun_zipper_res2.ply" );

    if( !mesh ) {
        fprintf( stderr, "Error loading mesh\n" );
        return;
    }

    AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();

    //mesh->material = new DiffuseMaterial( 0.75, 1.0, 0.8 );
    mesh->material = new MirrorMaterial();

    TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
    mesh_octree->build();
    mesh->accelerator = mesh_octree;
    mesh->transform = new Transform();
    *mesh->transform = compose( makeScaling( 2, 2, 2 ),
                                makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
    container->add( mesh );

    Sphere * sphere = nullptr;

    sphere = new Sphere( -2, 0.25, 2.5, 0.25 );
    sphere->material = new DiffuseMaterial( 1.0, 0.5, 0.5 );
    container->add( sphere );

    sphere = new Sphere( -2, 0.75, 0, 0.75 );
    sphere->material = new MirrorMaterial();
    container->add( sphere );

    sphere = new Sphere( +2.5, 1.00, 0, 1.00 );
    sphere->material = new DiffuseMaterial( 0.5, 0.5, 1.0 );
    container->add( sphere );

    sphere = new Sphere( -0.75, 0.50, 4.0, 0.50 );
    sphere->material = new MirrorMaterial( 1.0, 0.2, 1.0 );
    container->add( sphere );

    AxisAlignedSlab * cube = nullptr;

    cube = new AxisAlignedSlab( +0.50, 0, 5.0, 1.0 );
    cube->material = new DiffuseMaterial( 0.5, 1.0, 0.5 );
    container->add( cube );

	scene->root = container;
    scene->env_map = new ArcLightEnvironmentMap();
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_reflect3_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -0.2, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

// ------------------------------------------------------------ 
// Refraction
// ------------------------------------------------------------ 
void testRefraction1()
{
    //int imageSize = 32;
    //int imageSize = 100;
    int imageSize = 320;
    //int imageSize = 1024;
    int imageWidth = imageSize, imageHeight = imageSize;
    //int anim_frames = 10;
    int anim_frames = 1;
    int samples_per_pixel = 100;
    //int samples_per_pixel = 20;
    ImageTracer tracer( imageWidth, imageHeight, anim_frames, samples_per_pixel );
    tracer.loopable_animations = true;
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    Sphere * sphere = nullptr;

    //sphere = new Sphere( -2, 0.25, 0.5, 0.25 );
    //sphere->material = new DiffuseMaterial( 1.0, 0.5, 0.5 );
    //container->add( sphere );

    sphere = new Sphere( 0, 0.50, 1.5, 0.50 );
    sphere->material = new RefractiveMaterial(1.2);
    //sphere->material = new RefractiveMaterial(1.5);
    container->add( sphere );

    sphere = new Sphere( -1, 1.50, 10.0, 0.50 );
    //sphere->material = new RefractiveMaterial(1.2);
    sphere->material = new RefractiveMaterial(1.5);
    container->add( sphere );

    sphere = new Sphere( -1, 0.75, 0, 0.75  );
    sphere->material = new MirrorMaterial();
    container->add( sphere );

    //sphere = new Sphere( +1, 1.00, 0, 1.00 );
    //sphere->material = new DiffuseMaterial( 0.5, 0.5, 1.0 );
    //container->add( sphere );

    auto cube = new AxisAlignedSlab( -0.75, -0.75, -0.75, 1.5 );
    cube->material = new RefractiveMaterial(1.2);
    container->add( cube );
    cube->transform = new TimeVaryingTransform(
        [](float anim_progress) {
            return compose( makeTranslation( Vector4( 1.75, 1.2, 2.75 ) ),
                            //makeRotation( anim_progress * 0.5 * M_PI + 0.6, Vector4(0, 1, 0) ) );
                            makeRotation( anim_progress * 0.5 * M_PI, Vector4(0, 1, 0) ) );
                            
        });

#if 0
    cube = new AxisAlignedSlab( -0.50, -0.50, -0.50, 1.0 );
    cube->material = new RefractiveMaterial(1.2);
    container->add( cube );
    cube->transform = new TimeVaryingTransform(
        [](float anim_progress) {
            return compose( makeTranslation( Vector4( -1.50, 1.00, 6.50 ) ),
                            makeRotation( anim_progress * 0.5 * M_PI, Vector4(1, 0, 0) ) );
                            
        });
#endif

    // Colored strips to show refraction from background objects
    cube = new AxisAlignedSlab( -10.0, 0.0, -2.0,
                                +10.0, 0.15, -2.15 );
    cube->material = new DiffuseMaterial( 0.5, 0.5, 1.0 );
    container->add( cube );

    cube = new AxisAlignedSlab( -10.0, 0.0, -0.0,
                                +10.0, 0.15, -0.15 );
    cube->material = new DiffuseMaterial( 1.0, 0.5, 0.0 );
    container->add( cube );

    cube = new AxisAlignedSlab( -10.0, 0.0, 2.0,
                                +10.0, 0.15, 2.15 );
    cube->material = new DiffuseMaterial( 0.0, 1.0, 0.5 );
    container->add( cube );
	scene->root = container;

    scene->env_map = new ArcLightEnvironmentMap(Vector4(0, 1, 0), 0.25 * M_PI);
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_refract1_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -0.2, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

void testRefraction2()
{
    //int imageSize = 64;
    int imageSize = 256;
    //int imageSize = 320;
    //int imageSize = 512;
    //int imageSize = 1024;
    int imageWidth = imageSize * 4, imageHeight = imageSize;
    //int rays_per_pixel = 10;
    int rays_per_pixel = 30;
    //int rays_per_pixel = 100;
    ImageTracer tracer( imageWidth, imageHeight, 1, rays_per_pixel );
    tracer.camera.xmin = -0.45;
    tracer.camera.xmax = 0.45;
    tracer.camera.ymin = -0.15;
    tracer.camera.ymax = 0.15;
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    AssetLoader loader;
    TriangleMesh * mesh = nullptr;

    Transform rotation = makeRotation( M_PI / 4.0, Vector4(0, 1, 0) );
    float spacing = 2.0;

    // high res
    std::string model = "models/stanford/bunny/reconstruction/bun_zipper.ply";
    // low res
    //std::string model = "models/stanford/bunny/reconstruction/bun_zipper_res4.ply";

    {
        mesh = loader.load( model );
        if( !mesh ) { fprintf( stderr, "Error loading mesh\n" ); return; }

        AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();

        mesh->material = new RefractiveMaterial(N_AIR);

        TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
        mesh_octree->build();
        mesh->accelerator = mesh_octree;
        mesh->transform = new Transform();
        *mesh->transform = compose( makeTranslation( Vector4( -2.0 * spacing, 0.1, 2.0 ) ),
                                    makeScaling( 3, 3, 3 ),
                                    rotation,
                                    makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
        container->add( mesh );
    }

    {
        mesh = loader.load( model );
        if( !mesh ) { fprintf( stderr, "Error loading mesh\n" ); return; }

        AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();

        mesh->material = new RefractiveMaterial(N_WATER);

        TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
        mesh_octree->build();
        mesh->accelerator = mesh_octree;
        mesh->transform = new Transform();
        *mesh->transform = compose( makeTranslation( Vector4( -1.0 * spacing, 0.1, 2.0 ) ),
                                    makeScaling( 3, 3, 3 ),
                                    rotation,
                                    makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
        container->add( mesh );
    }

    {
        mesh = loader.load( model );
        if( !mesh ) { fprintf( stderr, "Error loading mesh\n" ); return; }

        AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();

        mesh->material = new RefractiveMaterial(N_PLEXIGLAS);

        TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
        mesh_octree->build();
        mesh->accelerator = mesh_octree;
        mesh->transform = new Transform();
        *mesh->transform = compose( makeTranslation( Vector4( 0.0, 0.1, 2.0 ) ),
                                    makeScaling( 3, 3, 3 ),
                                    rotation,
                                    makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
        container->add( mesh );
    }

    {
        mesh = loader.load( model );
        if( !mesh ) { fprintf( stderr, "Error loading mesh\n" ); return; }

        AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();

        mesh->material = new RefractiveMaterial(N_FLINT_GLASS);

        TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
        mesh_octree->build();
        mesh->accelerator = mesh_octree;
        mesh->transform = new Transform();
        *mesh->transform = compose( makeTranslation( Vector4( 1.0 * spacing, 0.1, 2.0 ) ),
                                    makeScaling( 3, 3, 3 ),
                                    rotation,
                                    makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
        container->add( mesh );
    }

    {
        mesh = loader.load( model );
        if( !mesh ) { fprintf( stderr, "Error loading mesh\n" ); return; }

        AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();

        mesh->material = new RefractiveMaterial(N_DIAMOND);

        TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
        mesh_octree->build();
        mesh->accelerator = mesh_octree;
        mesh->transform = new Transform();
        *mesh->transform = compose( makeTranslation( Vector4( 2.0 * spacing, 0.1, 2.0 ) ),
                                    makeScaling( 3, 3, 3 ),
                                    rotation,
                                    makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
        container->add( mesh );
    }

    // Walls
#if 1
    auto wall = new AxisAlignedSlab( -10.0, 0.0, -10.0,
                                     +10.0, 10.0, -10.0 );
    wall->material = new DiffuseMaterial( 0.8, 0.8, 1.0 );
    container->add( wall );
#endif

    // Colored strips to show refraction from background objects
    auto cube = new AxisAlignedSlab( -10.0, 0.0, -2.0,
                                     +10.0, 0.15, -2.15 );
    cube->material = new DiffuseMaterial( 0.5, 0.5, 1.0 );
    container->add( cube );

    cube = new AxisAlignedSlab( -10.0, 0.5 + 0.0, -2.0,
                                +10.0, 0.5 + 0.15, -2.15 );
    cube->material = new DiffuseMaterial( 1.0, 0.5, 0.0 );
    container->add( cube );

    cube = new AxisAlignedSlab( -10.0, 1.0 + 0.0, -2.0,
                                +10.0, 1.0 + 0.15, -2.15 );
    cube->material = new DiffuseMaterial( 0.0, 1.0, 0.5 );
    container->add( cube );
	scene->root = container;

	scene->root = container;
    scene->env_map = new ArcLightEnvironmentMap();
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();
    //tracer.shader = new AmbientOcclusionShader(); // TEMP

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_refract2_";

    // Camera back and rotated a bit around x so we're looking slightly down
    {
    Transform rotation = makeRotation( -0.2, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 1.0, 15.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );
    }

    tracer.scene->buildLightList();
    tracer.render();
}

void testRefraction3()
{
    //int imageSize = 64;
    int imageSize = 256;
    //int imageSize = 320;
    //int imageSize = 512;
    //int imageSize = 1024;
    int imageWidth = imageSize * 4, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 100 );
    tracer.camera.xmin = -0.45;
    tracer.camera.xmax = 0.45;
    tracer.camera.ymin = -0.15;
    tracer.camera.ymax = 0.15;
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    Sphere * sphere = nullptr;
    float y = 1.5, z = 2.0, radius = 1.0;

    sphere = new Sphere( -4, y, z, radius );
    sphere->material = new RefractiveMaterial(1.3);
    container->add( sphere );

    sphere = new Sphere( -2, y, z, radius );
    sphere->material = new RefractiveMaterial(1.49);
    container->add( sphere );

    sphere = new Sphere( 0, y, z, radius );
    sphere->material = new RefractiveMaterial(1.6);
    container->add( sphere );

    sphere = new Sphere( +2, y, z, radius );
    sphere->material = new RefractiveMaterial(2.1);
    container->add( sphere );

    sphere = new Sphere( +4, y, z, radius );
    sphere->material = new RefractiveMaterial(2.5);
    container->add( sphere );

    // Colored strips to show refraction from background objects
    auto cube = new AxisAlignedSlab( -10.0, 0.0, -2.0,
                                     +10.0, 0.15, -2.15 );
    cube->material = new DiffuseMaterial( 0.5, 0.5, 1.0 );
    container->add( cube );

    cube = new AxisAlignedSlab( -10.0, 0.5 + 0.0, -2.0,
                                +10.0, 0.5 + 0.15, -2.15 );
    cube->material = new DiffuseMaterial( 1.0, 0.5, 0.0 );
    container->add( cube );

    cube = new AxisAlignedSlab( -10.0, 1.0 + 0.0, -2.0,
                                +10.0, 1.0 + 0.15, -2.15 );
    cube->material = new DiffuseMaterial( 0.0, 1.0, 0.5 );
    container->add( cube );
	scene->root = container;

	scene->root = container;
    scene->env_map = new ArcLightEnvironmentMap(Vector4(0, 1, 0), M_PI * 0.25);
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_refract3_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -0.2, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 1.0, 15.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

void testRefraction4()
{
    //int imageSize = 512;
    int imageSize = 256;
    //int imageSize = 128;
    int imageWidth = imageSize, imageHeight = imageSize;
    int rays_per_pixel = 100;
    ImageTracer tracer( imageWidth, imageHeight, 1, rays_per_pixel );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, 0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    auto sphere = new Sphere( 0.0, 1.25, 0.0, 1.0 );
    sphere->material = new RefractiveMaterial(1.4);
    container->add( sphere );

    auto foo = new AxisAlignedSlab( -0.5, -0.5, -0.5, 1.0 );
    foo->material = new RefractiveMaterial(1.1);
    foo->transform = new Transform();
    *foo->transform = compose( makeTranslation( Vector4( 1.5, 0.75, 0.0 ) ),
                                //makeRotation( -0.8 * M_PI, Vector4(0, 1, 0) ) );
                                makeRotation( 0.0, Vector4(0, 1, 0) ) );
    container->add( foo );

    // Light
    auto cube = new AxisAlignedSlab( 2.0, 2.5, -1.0,
                                     4.0, 4.5,  1.0 );
    cube->material = new Material();
    cube->material->emittance = RGBColor( 1.0, 1.0, 1.0 );
    cube->material->emittance.scale( 5.0 );
    container->add( cube );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_refract4_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}


void testMesh1()
{
    int imageSize = 256;
    //int imageSize = 320;
    //int imageSize = 512;
    //int imageSize = 1024;
    int imageWidth = imageSize * 2, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 25 );
    tracer.camera.xmin = -0.3;
    tracer.camera.xmax = 0.3;
    tracer.camera.ymin = -0.15;
    tracer.camera.ymax = 0.15;
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    AssetLoader loader;
    TriangleMesh * mesh = nullptr;

    {
        mesh = loader.load( "models/stanford/bunny/reconstruction/bun_zipper.ply" );
        if( !mesh ) { fprintf( stderr, "Error loading mesh\n" ); return; }

        AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();

        //mesh->material = new DiffuseMaterial( 0.75, 1.0, 0.8 );
        //mesh->material = new MirrorMaterial();

        TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
        mesh_octree->build();
        mesh->accelerator = mesh_octree;
        mesh->transform = new Transform();
        *mesh->transform = compose( makeScaling( 2, 2, 2 ),
                                    makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
        container->add( mesh );
    }

    {
        mesh = loader.load( "models/stanford/dragon/reconstruction/dragon_vrip.ply" );
        if( !mesh ) { fprintf( stderr, "Error loading mesh\n" ); return; }
        AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();

        TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
        mesh_octree->build();
        mesh->accelerator = mesh_octree;
        mesh->transform = new Transform();
        *mesh->transform = compose( makeTranslation( 3.0, 0.0, 0.0 ),
                                    compose( makeScaling( 4, 4, 4 ),
                                             makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) ) );;
        container->add( mesh );
    }

	scene->root = container;
    scene->env_map = new ArcLightEnvironmentMap();
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_mesh1_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -0.2, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 1.0, 15.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

// Multipart mesh
void testMesh2()
{
    //int imageSize = 100;
    int imageSize = 256;
    //int imageSize = 320;
    //int imageSize = 512;
    //int imageSize = 1024;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 10 );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    AssetLoader loader;
    auto meshes = loader.loadMultiPart( "models/nasa/lunarlandernofoil-c/lunarlandernofoil_carbajal.3ds" );
    //auto meshes = loader.loadMultiPartMerged( "models/nasa/lunarlandernofoil-c/lunarlandernofoil_carbajal.3ds" );
    if( !meshes ) { fprintf( stderr, "Error loading meshes\n" ); return; }
    meshes->transform = new Transform();
    *meshes->transform = makeRotation( -0.5 * M_PI, Vector4( 1.0, 0.0, 0.0 ) );
    container->add( meshes );

	scene->root = container;
    //scene->env_map = new ArcLightEnvironmentMap();
    tracer.scene = scene;

    scene->addPointLight( PointLight( Vector4( 12.0, 5.0, 5.0 ),
        RGBColor( 1.0, 1.0, 1.0 ).scaled(60.0) ) );
    scene->addPointLight( PointLight( Vector4( -15.0, 5.0, 2.0 ),
        RGBColor( 1.0, 0.8, 1.0 ).scaled(40.0) ) );

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_mesh2_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -0.1 * M_PI, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 2.0, 25.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

// Big multipart mesh
void testMeshSanMiguel()
{
    //int imageSize = 50;
    //int imageSize = 100;
    //int imageSize = 256;
    //int imageSize = 320;
    int imageSize = 512;
    //int imageSize = 1024;
    int imageWidth = imageSize, imageHeight = imageSize;
    //ImageTracer tracer( imageWidth, imageHeight, 1, 10 );
    ImageTracer tracer( imageWidth, imageHeight, 1, 1 );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    //AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
    //                                               +10.0, -1.0, -10.0 );
    //container->add( floor );

    AssetLoader loader;
    auto mesh = loader.loadMultiPart( "models/san-miguel/san-miguel.obj" );
    //auto mesh = loader.loadMultiPartMerged( "models/san-miguel/san-miguel.obj" );
    if( !mesh ) { fprintf( stderr, "Error loading meshes\n" ); return; }
    //AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();
    mesh->transform = new Transform();
    //float scale = 35.0;
    float scale = 1.0;
    *mesh->transform = compose( makeScaling( scale, scale, scale ),
                                makeRotation( M_PI / 2, Vector4( 0.0, 1.0, 0.0 ) )//,
                              );
                               // makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
    container->add( mesh );

    auto s = new Sphere( -1.25, 0.25, 0.75, 0.25 );
    s->material = new DiffuseMaterial(1, 1, 0);
    container->add( s );
    s = new Sphere( 1.25, 0.25, 0.75, 0.25 );
    s->material = new DiffuseMaterial(0, 1, 0);
    container->add( s );
    s = new Sphere( 0.0, 0.25, 3.0, 0.25 );
    s->material = new DiffuseMaterial(0, 1, 1);
    container->add( s );

    scene->addPointLight( PointLight( Vector4( 20.0, 20.0, -15.0 ),
        RGBColor( 1.0, 1.0, 1.0 ).scaled(150.0) ) );
    scene->addPointLight( PointLight( Vector4( 20.0, 20.0, 10.0 ),
        RGBColor( 1.0, 1.0, 1.0 ).scaled(1300.0) ) );
    scene->addPointLight( PointLight( Vector4( 0.0, 20.0, 5.0 ),
        RGBColor( 1.0, 1.0, 1.0 ).scaled(500.0) ) );

	scene->root = container;
    //scene->env_map = new ArcLightEnvironmentMap();
    tracer.scene = scene;

    //tracer.shader = new BasicDiffuseSpecularShader();
    tracer.shader = new GoochShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_mesh_san_miguel_";

#if 0 // TEMP
    Transform rotation = makeRotation( -0.13 * M_PI, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 100.0 );
    tracer.setCameraTransform( compose( makeTranslation( 1.8, 0.0, -40.0 ),
                                        rotation,
                                        translation ) );
#elif 0
    // FIXME - Something is wrong with the geometry from this angle
    Transform rotation = makeRotation( -0.13 * M_PI, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 80.0 );
    tracer.setCameraTransform( compose( makeTranslation( 1.8, -2.0, -40.0 ),
                                        makeRotation( 0.35, Vector4(0, 1, 0) ), // turn about Y
                                        rotation,
                                        translation ) );
#else
    // Over archway 
    Transform rotation = makeRotation( -0.13 * M_PI, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 40.0 );
    tracer.setCameraTransform( compose( makeTranslation( 1.8, -2.0, -40.0 ),
                                        rotation,
                                        translation ) );
#endif

    tracer.scene->buildLightList();
    tracer.render();
}


void testHairball()
{
    //int imageSize = 32;
    //int imageSize = 320;
    int imageSize = 512;
    //int imageSize = 1024;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 10 );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    AssetLoader loader;
    TriangleMesh * mesh = nullptr;

    {
        mesh = loader.load( "models/hairball.obj" );
        if( !mesh ) { fprintf( stderr, "Error loading mesh\n" ); return; }

        AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();

        //mesh->material = new DiffuseMaterial( 0.75, 1.0, 0.8 );
        //mesh->material = new MirrorMaterial();

        TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
        mesh_octree->build();
        mesh->accelerator = mesh_octree;
        mesh->transform = new Transform();
        *mesh->transform = compose( makeScaling( 2, 2, 2 ),
                                    makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
        container->add( mesh );
    }

	scene->root = container;
    //scene->env_map = new ArcLightEnvironmentMap();
    scene->env_map = new ArcLightEnvironmentMap(Vector4(1,1,0), M_PI / 4.0f);
    tracer.scene = scene;

    tracer.shader = new AmbientOcclusionShader();
    //tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_hairball_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -0.2, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 1.0, 10.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}


// ------------------------------------------------------------ 
// Animation
// ------------------------------------------------------------ 
void testAnimTransforms1()
{
    int imageSize = 512;
    int imageWidth = imageSize, imageHeight = imageSize;
    int rays_per_pixel = 10;
    int num_frames = 15;
    ImageTracer tracer( imageWidth, imageHeight, num_frames, rays_per_pixel );
    tracer.loopable_animations = true;
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane at y=0
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    AssetLoader loader;
    std::string modelBasePath = "models";
    std::string modelPath = modelBasePath + "/stanford/bunny/reconstruction";
    TriangleMesh * mesh = loader.load( modelPath + "/bun_zipper_res2.ply" );

    if( !mesh ) {
        fprintf( stderr, "Error loading mesh\n" );
        return;
    }

    AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();

    mesh->material = new MirrorMaterial();

    TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
    mesh_octree->build();
    mesh->accelerator = mesh_octree;
    float ymin = bounds->ymin;
    mesh->transform = new TimeVaryingTransform(
        [ymin](float anim_progress) {
            return compose( makeScaling( 2, 2, 2 ),
                            makeRotation( anim_progress * 2.0 * M_PI, Vector4(0, 1, 0) ),
                            makeTranslation( Vector4( 0.0, -ymin, 0.0 ) ) );
        });
    container->add( mesh );

    container->add( new Sphere( -2, 0.25, 0, 0.25 ) );

    // colored balls to see if we are gettingn proper reflections
    auto s = new Sphere( -1.25, 0.25, 0.75, 0.25 );
    s->material = new DiffuseMaterial(1, 1, 0);
    container->add( s );
    s = new Sphere( 1.25, 0.25, 0.75, 0.25 );
    s->material = new DiffuseMaterial(0, 1, 0);
    container->add( s );
    s = new Sphere( 0.0, 0.25, 3.0, 0.25 );
    s->material = new DiffuseMaterial(0, 1, 1);
    container->add( s );

    auto cube = new AxisAlignedSlab( 0.8, 0, 2.0, 0.5 );
    cube->material = new DiffuseMaterial( 1.0, 0.2, 0.2 );
    container->add( cube );

    scene->addPointLight( PointLight( Vector4( -15.0, 15.0, 15.0 ),
        RGBColor( 1.0, 1.0, 1.0 ).scaled(150.0) ) );
    scene->addPointLight( PointLight( Vector4( 13.0, 15.0, 20.0 ),
        RGBColor( 1.0, 0.4, 0.4 ).scaled(150.0) ) );
    scene->addPointLight( PointLight( Vector4( 0.0, 20.0, 15.0 ),
        RGBColor( 0.4, 0.4, 1.0 ).scaled(150.0) ) );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_anim_xforms1_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

void testAnimTransforms2()
{
    //int imageSize = 512;
    int imageSize = 256;
    //int imageSize = 128;
    int imageWidth = imageSize, imageHeight = imageSize;
    int rays_per_pixel = 30;
    int num_frames = 30;
    //int num_frames = 1;
    ImageTracer tracer( imageWidth, imageHeight, num_frames, rays_per_pixel );
    tracer.loopable_animations = true;
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, -2.0, +10.0,
                                                   +10.0, -3.0, -10.0 );
    container->add( floor );

    auto cube = new AxisAlignedSlab( -0.5, -0.5, -0.5, 1.0 );
    cube->transform = new TimeVaryingTransform(
        [](float anim_progress) {
            return compose( makeTranslation( Vector4( -2.0, 0.0, 0.0 ) ),
                            makeRotation( (anim_progress - 0.4) * 2.0 * M_PI, Vector4(1, 0, 0) ) );
        });
    container->add( cube );
    cube = new AxisAlignedSlab( -0.5, -0.5, -0.5, 1.0 );
    cube->transform = new TimeVaryingTransform(
        [](float anim_progress) {
            return compose( makeTranslation( Vector4( 0.0, 0.0, 0.0 ) ),
                            makeRotation( (anim_progress - 0.4) * 2.0 * M_PI, Vector4(0, 1, 0) ) );
        });
    container->add( cube );
    cube = new AxisAlignedSlab( -0.5, -0.5, -0.5, 1.0 );
    cube->transform = new TimeVaryingTransform(
        [](float anim_progress) {
            return compose( makeTranslation( Vector4( +2.0, 0.0, 0.0 ) ),
                            makeRotation( (anim_progress - 0.4) * 2.0 * M_PI, Vector4(0, 0, 1) ) );
        });
    container->add( cube );

    scene->addPointLight( PointLight( Vector4( -15.0, 15.0, 15.0 ),
        RGBColor( 1.0, 1.0, 1.0 ).scaled(100.0) ) );
    scene->addPointLight( PointLight( Vector4( 5.0, 10.0, 5.0 ),
        RGBColor( 1.0, 1.0, 1.0 ).scaled(100.0) ) );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_anim_xforms2_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

void testAnimTransforms3()
{
    //int imageSize = 512;
    //int imageSize = 256;
    int imageSize = 128;
    int imageWidth = imageSize, imageHeight = imageSize;
    int rays_per_pixel = 10;
    int num_frames = 10;
    //int num_frames = 1;
    ImageTracer tracer( imageWidth, imageHeight, num_frames, rays_per_pixel );
    tracer.loopable_animations = true;
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, -2.0, +10.0,
                                                   +10.0, -3.0, -10.0 );
    container->add( floor );

    auto cube = new AxisAlignedSlab( -0.5, -0.5, -0.5, 1.0 );
    cube->material = new MirrorMaterial();
    cube->transform = new TimeVaryingTransform(
        [](float anim_progress) {
            return compose( makeTranslation( Vector4( -2.0, 0.0, 0.0 ) ),
                            makeRotation( (anim_progress - 0.4) * 0.5 * M_PI, Vector4(1, 0, 0) ) );
        });
    container->add( cube );
    cube = new AxisAlignedSlab( -0.5, -0.5, -0.5, 1.0 );
    cube->material = new MirrorMaterial();
    cube->transform = new TimeVaryingTransform(
        [](float anim_progress) {
            return compose( makeTranslation( Vector4( 0.0, 0.0, 0.0 ) ),
                            makeRotation( (anim_progress - 0.4) * 0.5 * M_PI, Vector4(0, 1, 0) ) );
        });
    container->add( cube );
    cube = new AxisAlignedSlab( -0.5, -0.5, -0.5, 1.0 );
    cube->material = new MirrorMaterial();
    cube->transform = new TimeVaryingTransform(
        [](float anim_progress) {
            return compose( makeTranslation( Vector4( +2.0, 0.0, 0.0 ) ),
                            makeRotation( (anim_progress - 0.4) * 0.5 * M_PI, Vector4(0, 0, 1) ) );
        });
    container->add( cube );

    scene->addPointLight( PointLight( Vector4( -15.0, 15.0, 15.0 ),
        RGBColor( 1.0, 1.0, 1.0 ).scaled(100.0) ) );
    scene->addPointLight( PointLight( Vector4( 5.0, 10.0, 5.0 ),
        RGBColor( 1.0, 1.0, 1.0 ).scaled(100.0) ) );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_anim_xforms3_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

// ------------------------------------------------------------ 
// Area Lights
// ------------------------------------------------------------ 
void testAreaLight1()
{
    //int imageSize = 512;
    int imageSize = 256;
    //int imageSize = 128;
    int imageWidth = imageSize, imageHeight = imageSize;
    int rays_per_pixel = 100;
    ImageTracer tracer( imageWidth, imageHeight, 1, rays_per_pixel );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, 0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    auto cube = new AxisAlignedSlab( -0.5, -0.5, -0.5, 1.0 );
    //cube->material = new MirrorMaterial();
    cube->transform = new Transform();
    *cube->transform = compose( makeTranslation( Vector4( 0.0, 0.5, 0.0 ) ),
                                makeRotation( -0.8 * M_PI, Vector4(0, 1, 0) ) );
    container->add( cube );

    // Light
    cube = new AxisAlignedSlab( 2.0, 2.5, -2.25,
                                3.0, 3.5,  2.25 );
    cube->material = new Material();
    cube->material->emittance = RGBColor( 1.0, 1.0, 1.0 );
    cube->material->emittance.scale( 5.0 );
    container->add( cube );

    ArcLightEnvironmentMap * env_map = new ArcLightEnvironmentMap( Vector4(0, 1, 0), M_PI / 2.0 );
    env_map->setPower( 10.0f );
    scene->env_map = env_map;

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_area_light1_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

// Caustics
void testAreaLight2()
{
    //int imageSize = 512;
    int imageSize = 256;
    //int imageSize = 128;
    int imageWidth = imageSize, imageHeight = imageSize;
    int rays_per_pixel = 1000;
    ImageTracer tracer( imageWidth, imageHeight, 1, rays_per_pixel );
    Scene * scene = new Scene();
	FlatContainer * container = new FlatContainer();

    // Ground plane
    AxisAlignedSlab * floor = new AxisAlignedSlab( -10.0, 0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    AxisAlignedSlab * cube = nullptr;

#if 0
    cube = new AxisAlignedSlab( -0.5, -0.5, -0.5, 1.0 );
    //cube->material = new MirrorMaterial();
    cube->transform = new Transform();
    *cube->transform = compose( makeTranslation( Vector4( -0.8, 0.5, 0.0 ) ),
                                makeRotation( -0.1 * M_PI, Vector4(0, 1, 0) ) );
    container->add( cube );
#endif

    int num_cubes = 20;
    float min_angle = -M_PI / 2.0;
    float max_angle = M_PI / 2.0;
    for( int i = 0; i < num_cubes; i++ ) {
        float angle = (float) i / (num_cubes-1) * (max_angle - min_angle) + min_angle;
        cube = new AxisAlignedSlab( -0.5, -0.5, -0.5, 1.0 );
        cube->material = new MirrorMaterial();
        cube->transform = new Transform();
        *cube->transform = compose( makeRotation( angle, Vector4(0, 1, 0) ),
                                    makeTranslation( Vector4( -2.0, 0.5, 0.0 ) ),
                                    makeScaling( 0.1, 1.0, 0.4 ) );
        container->add( cube );
    }

#if 0
    cube = new AxisAlignedSlab( -0.5, -0.5, -0.5, 1.0 );
    cube->transform = new Transform();
    *cube->transform = compose( makeTranslation( Vector4( 0.8, 0.5, 0.0 ) ),
                                makeRotation( -0.8 * M_PI, Vector4(0, 1, 0) ) );
    container->add( cube );
#endif


    // Light
    cube = new AxisAlignedSlab( 2.0, 2.5, -2.25,
                                3.0, 3.5,  2.25 );
    cube->material = new Material();
    cube->material->emittance = RGBColor( 1.0, 1.0, 1.0 );
    cube->material->emittance.scale( 5.0 );
    container->add( cube );

    //ArcLightEnvironmentMap * env_map = new ArcLightEnvironmentMap( Vector4(0, 1, 0), M_PI / 2.0 );
    //env_map->setPower( 10.0f );
    //scene->env_map = env_map;

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();
    //tracer.shader = new AmbientOcclusionShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_area_light2_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 4, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

// ------------------------------------------------------------ 
BEGIN_SCENE(SimpleCube)
SETUP_SCENE( TestScene::setup(); );
BUILD_SCENE(
    float size = 1.0;
    float half_size = size / 2.0;
    auto cube = new AxisAlignedSlab( -half_size, 0.0, -half_size,
                                     half_size, size, half_size );
    cube->material = new DiffuseMaterial( 0.5, 0.5, 1.0 );
    container->add( cube );

    scene->addPointLight( PointLight( Vector4( -3.0, 3.0, 3.0 ),
                                      RGBColor( 1.0, 1.0, 1.0 ).scaled(20.0) ) );

    tracer->shader = new BasicDiffuseSpecularShader();
);
END_SCENE()
// ------------------------------------------------------------ 
BEGIN_SCENE(Gooch)
SETUP_SCENE(
    TestScene::setup();
    tracer->rays_per_pixel = 4;
    tracer->shader = new GoochShader();
);
BUILD_SCENE(
    float size = 1.0;
    float half_size = size / 2.0;
    auto cube = new AxisAlignedSlab( -half_size, 0.0, -half_size,
                                     half_size, size, half_size );
    cube->transform = new Transform();
    *cube->transform = makeTranslation( Vector4( -1.0, 0.0, 0.0 ) );
    container->add( cube );

    auto sphere = new Sphere( 1.0, half_size, 0.0, half_size );
    container->add( sphere );

    std::string modelBasePath = "models";
    TriangleMesh * mesh = loader.load( modelBasePath + "/stanford/happy/reconstruction/happy_vrip_res4.ply" );
    AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();
    if( !mesh ) { fprintf( stderr, "Error loading mesh\n" ); return; }

    TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
    mesh_octree->build();
    mesh->accelerator = mesh_octree;
    mesh->transform = new Transform();
    //*mesh->transform = makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) );
    *mesh->transform = makeScaling( 3.0 );
    container->add( mesh );
);
END_SCENE()
// ------------------------------------------------------------ 

BEGIN_SCENE(MaterialTestBase)
SETUP_SCENE(
    TestScene::setup();
    tracer->rays_per_pixel = 1;
    tracer->shader = new GoochShader();
    tracer->setCameraTransform( compose(
        // move up a bit
        makeTranslation( 0.0, 0.5, 0.0 ),
        // rotate so we are looking down
        makeRotation( -0.2, Vector4(1, 0, 0) ),
        // back away from the origin
        makeTranslation( 0.0, 0.0, 5.0 )
        ) );
);
TriangleMesh * mesh = nullptr;
BUILD_SCENE(
    float size = 1.0;
    auto floor = new AxisAlignedSlab( -10, -1, -10,
                                     10, 0, 10 );
    floor->transform = new Transform();
    *floor->transform = makeTranslation( Vector4( -1.0, 0.0, 0.0 ) );
    container->add( floor );

    mesh = loadMaterialTestModel( loader );
    if( !mesh ) { return; }
    AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();

    TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
    mesh_octree->build();
    mesh->accelerator = mesh_octree;
    mesh->transform = new Transform();
    *mesh->transform = compose( makeScaling( 1.0 ),
                                makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
    container->add( mesh );
);
END_SCENE()
// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestAmbientOcclusion, MaterialTestBase)
SETUP_SCENE(
    MaterialTestBase::setup();
    tracer->rays_per_pixel = 30;
    tracer->shader = new AmbientOcclusionShader();
);
BUILD_SCENE(
    MaterialTestBase::buildScene();
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestPointLight, MaterialTestBase)
SETUP_SCENE(
    MaterialTestBase::setup();
    tracer->rays_per_pixel = 10;
);
BUILD_SCENE(
    MaterialTestBase::buildScene();
    scene->addPointLight( PointLight( Vector4( -5.0, 5.0, 5.0 ),
                          RGBColor( 1.0, 1.0, 1.0 ).scaled(30.0) ) );
    scene->addPointLight( PointLight( Vector4( 5.0, 5.0, 5.0 ),
                          RGBColor( 1.0, 1.0, 1.0 ).scaled(30.0) ) );
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestArcLight, MaterialTestBase)
SETUP_SCENE(
    MaterialTestBase::setup();
    tracer->rays_per_pixel = 30;
);
BUILD_SCENE(
    MaterialTestBase::buildScene();
    scene->env_map = new ArcLightEnvironmentMap();
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestHDREnvironmentMap, MaterialTestBase)
SETUP_SCENE(
    MaterialTestBase::setup();
    tracer->rays_per_pixel = 30;
    //tracer->camera.xmin = -0.25;// TEMP
    //tracer->camera.xmax = +0.25;// TEMP
    //tracer->camera.ymin = -0.25;// TEMP
    //tracer->camera.ymax = +0.25;// TEMP
);
BUILD_SCENE(
    MaterialTestBase::buildScene();
    scene->env_map = new HDRImageEnvironmentMap(env_map_filename, env_map_width, env_map_height);
);
#if 1
std::string env_map_filename = "light_probes/debevec/stpeters_probe.float";
unsigned int env_map_width = 1500;
unsigned int env_map_height = 1500;
#else
std::string env_map_filename = "light_probes/debevec/grace_probe.float";
unsigned int env_map_width = 1000;
unsigned int env_map_height = 1000;
#endif
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestDiffuseWhitePointLight, MaterialTestPointLight)
SETUP_SCENE(
    MaterialTestPointLight::setup();
    tracer->shader = new BasicDiffuseSpecularShader();
);
BUILD_SCENE(
    MaterialTestPointLight::buildScene();
    mesh->material = new DiffuseMaterial( 1.0, 1.0, 1.0 );
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestMirrorPointLight, MaterialTestPointLight)
SETUP_SCENE(
    MaterialTestPointLight::setup();
    tracer->shader = new BasicDiffuseSpecularShader();
);
BUILD_SCENE(
    MaterialTestPointLight::buildScene();
    mesh->material = new MirrorMaterial();
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestRefractPointLight, MaterialTestPointLight)
SETUP_SCENE(
    MaterialTestPointLight::setup();
    tracer->shader = new BasicDiffuseSpecularShader();
);
BUILD_SCENE(
    MaterialTestPointLight::buildScene();
    mesh->material = new RefractiveMaterial(N_WATER);
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestDiffuseWhiteArcLight, MaterialTestArcLight)
SETUP_SCENE(
    MaterialTestArcLight::setup();
    tracer->shader = new BasicDiffuseSpecularShader();
);
BUILD_SCENE(
    MaterialTestArcLight::buildScene();
    mesh->material = new DiffuseMaterial( 1.0, 1.0, 1.0 );
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestMirrorArcLight, MaterialTestArcLight)
SETUP_SCENE(
    MaterialTestArcLight::setup();
    tracer->shader = new BasicDiffuseSpecularShader();
);
BUILD_SCENE(
    MaterialTestArcLight::buildScene();
    mesh->material = new MirrorMaterial();
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestRefractWaterArcLight, MaterialTestArcLight)
SETUP_SCENE(
    MaterialTestArcLight::setup();
    tracer->shader = new BasicDiffuseSpecularShader();
);
BUILD_SCENE(
    MaterialTestArcLight::buildScene();
    mesh->material = new RefractiveMaterial(N_WATER);
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestRefractDiamondArcLight, MaterialTestArcLight)
SETUP_SCENE(
    MaterialTestArcLight::setup();
    tracer->shader = new BasicDiffuseSpecularShader();
);
BUILD_SCENE(
    MaterialTestArcLight::buildScene();
    mesh->material = new RefractiveMaterial(N_DIAMOND);
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestDiffuseWhiteHDREnvironmentMap, MaterialTestHDREnvironmentMap)
SETUP_SCENE(
    MaterialTestHDREnvironmentMap::setup();
    tracer->shader = new BasicDiffuseSpecularShader();
);
BUILD_SCENE(
    MaterialTestHDREnvironmentMap::buildScene();
    mesh->material = new DiffuseMaterial( 1.0, 1.0, 1.0 );
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestMirrorHDREnvironmentMap, MaterialTestHDREnvironmentMap)
SETUP_SCENE(
    MaterialTestHDREnvironmentMap::setup();
    tracer->shader = new BasicDiffuseSpecularShader();
);
BUILD_SCENE(
    MaterialTestHDREnvironmentMap::buildScene();
    mesh->material = new MirrorMaterial();
);
END_SCENE()


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
#if 0
    // Ambient occlusion
    testAO1();
    testAO2();
    testAO3();
    testAO4();
    testAO5();
    testAO6();
    testAO7();
    testSphereLight1();
    testSphereLight2();
    testSphereLight3();
    testSphereLight4();
    testReflection1();
    testReflection2();
    testReflection3();
    testMesh1();
    testMesh2();
    //testMeshSanMiguel();      // TODO: slow - san miguel scene
    //testHairball();   // TODO: slow
    testPointLight1();
    testPointLight2();
    testPointLight3();
    testPointLight4();
    testAnimTransforms1();
    testAnimTransforms2();
    testAnimTransforms3();
    testAreaLight1();
    testAreaLight2();
    testRefraction1();  // Mixed scene with some refractive elements
    testRefraction2();  // Mesh bunnies with varying IoR
    testRefraction3();  // Spheres of varying IoR
    testRefraction4();  // Refractive sphere with caustics
    SimpleCube::run();
    Gooch::run();
    //MaterialTestBase::run();
    MaterialTestAmbientOcclusion::run();
    MaterialTestDiffuseWhitePointLight::run();
    MaterialTestMirrorPointLight::run();
    MaterialTestRefractPointLight::run();
    MaterialTestDiffuseWhiteArcLight::run();
    MaterialTestMirrorArcLight::run();
    MaterialTestRefractWaterArcLight::run();
    MaterialTestRefractDiamondArcLight::run();
    MaterialTestDiffuseWhiteHDREnvironmentMap::run(); // TODO - Needs work, probably importance sampling
    MaterialTestMirrorHDREnvironmentMap::run(); // TODO - Needs work, probably importance sampling
#else
    testAO3();
#endif
    
    total_run_timer.stop();
    printf("Done - Run time = %f seconds\n", total_run_timer.elapsed());
    fflush(stdout);
    return 0;
}

