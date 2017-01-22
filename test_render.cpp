#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>

#include <sys/stat.h>
#include <stdio.h>
#include <math.h>

#include "FastRender.h"

AssetLoader loader;
RandomNumberGenerator rng;
std::string output_path = "testoutput";
unsigned int plot_size = 500;

const std::string modelBasePath = "models";

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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    container->add( std::make_shared<Sphere>( -2, 0.25, 0, 0.25 ) );
    container->add( std::make_shared<Sphere>( -1, 0.50, 0, 0.50 ) );
    container->add( std::make_shared<Sphere>( +1, 1.00, 0, 1.00 ) );

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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    container->add( std::make_shared<Sphere>( -2, 0.25, 0, 0.25 ) );
    container->add( std::make_shared<Sphere>( -1, 0.50, 0, 0.50 ) );
    container->add( std::make_shared<Sphere>( +1, 1.00, 0, 1.00 ) );

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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    container->add( std::make_shared<AxisAlignedSlab>( +1.75, 0, 0, 1.25 ) );
    container->add( std::make_shared<AxisAlignedSlab>( +0.0, 0, 0, 1.00 ) );
    container->add( std::make_shared<AxisAlignedSlab>( -1.5, 0, 0, 0.75 ) );
    container->add( std::make_shared<AxisAlignedSlab>( -2.5, 0, 0, 0.50 ) );

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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    std::shared_ptr<AxisAlignedSlab> cube;

    cube = std::make_shared<AxisAlignedSlab>( +1.75, 0, 0, 1.25 );
    cube->material = std::make_shared<DiffuseMaterial>( 1.0, 1.0, 0.5 );
    container->add( cube );

    cube = std::make_shared<AxisAlignedSlab>( +0.0, 0, 0, 1.00 );
    cube->material = std::make_shared<DiffuseMaterial>( 1.0, 0.5, 0.5 );
    container->add( cube );

    cube = std::make_shared<AxisAlignedSlab>( -1.5, 0, 0, 0.75 );
    cube->material = std::make_shared<DiffuseMaterial>( 0.5, 0.5, 1.0 );
    container->add( cube );

    cube = std::make_shared<AxisAlignedSlab>( -2.5, 0, 0, 0.50 );
    cube->material = std::make_shared<DiffuseMaterial>( 0.5, 1.0, 0.5 );
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    auto cube = nullptr;

    std::string modelPath = modelBasePath + "/stanford/bunny/reconstruction";
    auto mesh = loader.load( modelPath + "/bun_zipper_res2.ply" );
    auto bounds = mesh->getAxisAlignedBounds();
    mesh->material = std::make_shared<DiffuseMaterial>( 1.0, 1.0, 1.0 );

    mesh->transform = std::make_shared<Transform>();
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    auto cube = nullptr;

    std::string modelPath = modelBasePath + "/stanford/dragon/reconstruction";
    auto mesh = loader.load( modelPath + "/dragon_vrip_res2.ply" );
    auto bounds = mesh->getAxisAlignedBounds();

    mesh->material = std::make_shared<DiffuseMaterial>( 1.0, 1.0, 1.0 );

    mesh->transform = std::make_shared<Transform>();
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    auto cube = nullptr;

    std::string modelPath = modelBasePath + "/stanford/happy/reconstruction";
    auto mesh = loader.load( modelPath + "/happy_vrip_res2.ply" );
    auto bounds = mesh->getAxisAlignedBounds();

    mesh->material = std::make_shared<DiffuseMaterial>( 1.0, 1.0, 1.0 );

    mesh->transform = std::make_shared<Transform>();
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    container->add( std::make_shared<Sphere>( -2, 0.25, 0, 0.25 ) );
    container->add( std::make_shared<Sphere>( -1, 0.50, 0, 0.50 ) );
    container->add( std::make_shared<Sphere>( +1, 1.00, 0, 1.00 ) );

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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    container->add( std::make_shared<Sphere>( -2, 0.25, 0, 0.25 ) );
    container->add( std::make_shared<Sphere>( -1, 0.50, 0, 0.50 ) );
    container->add( std::make_shared<Sphere>( +1, 1.00, 0, 1.00 ) );

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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    std::shared_ptr<Sphere> sphere;

    sphere = std::make_shared<Sphere>( -2, 0.25, 0, 0.25 );
    sphere->material = std::make_shared<DiffuseMaterial>( 1.0, 0.5, 0.5 );
    container->add( sphere );

    sphere = std::make_shared<Sphere>( -1, 0.50, 0, 0.50 );
    sphere->material = std::make_shared<DiffuseMaterial>( 0.5, 1.0, 0.5 );
    container->add( sphere );

    sphere = std::make_shared<Sphere>( +1, 1.00, 0, 1.00 );
    sphere->material = std::make_shared<DiffuseMaterial>( 0.5, 0.5, 1.0 );
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    container->add( std::make_shared<Sphere>( -2, 0.25, 0, 0.25 ) );
    container->add( std::make_shared<Sphere>( -1, 0.50, 0, 0.50 ) );
    container->add( std::make_shared<Sphere>( +1, 1.00, 0, 1.00 ) );

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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    container->add( std::make_shared<Sphere>( -2, 0.25, 0, 0.25 ) );
    container->add( std::make_shared<Sphere>( -1, 0.50, 0, 0.50 ) );
    container->add( std::make_shared<Sphere>( +1, 1.00, 0, 1.00 ) );

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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    container->add( std::make_shared<Sphere>( -2, 0.25, 0, 0.25 ) );
    container->add( std::make_shared<Sphere>( -1, 0.50, 0, 0.50 ) );
    container->add( std::make_shared<Sphere>( +1, 1.00, 0, 1.00 ) );

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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    std::string modelPath = modelBasePath + "/stanford/bunny/reconstruction";
    auto mesh = loader.load( modelPath + "/bun_zipper_res2.ply" );
    auto bounds = mesh->getAxisAlignedBounds();

    //mesh->material = std::make_shared<DiffuseMaterial>( 0.75, 1.0, 0.8 );

    mesh->transform = std::make_shared<Transform>();
    *mesh->transform = compose( makeScaling( 2, 2, 2 ),
                                makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
    container->add( mesh );

    container->add( std::make_shared<Sphere>( -2, 0.25, 0, 0.25 ) );

    auto cube = std::make_shared<AxisAlignedSlab>( 0.8, 0, 2.0, 0.5 );
    cube->material = std::make_shared<DiffuseMaterial>( 1.0, 0.2, 0.2 );
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    std::string modelPath = modelBasePath + "/stanford/bunny/reconstruction";
    auto mesh = loader.load( modelPath + "/bun_zipper_res2.ply" );
    auto bounds = mesh->getAxisAlignedBounds();

    mesh->material = std::make_shared<MirrorMaterial>();

    mesh->transform = std::make_shared<Transform>();
    *mesh->transform = compose( makeScaling( 2, 2, 2 ),
                                makeRotation( 0.25 * M_PI, Vector4(0, 1, 0) ),
                                makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
    container->add( mesh );

    container->add( std::make_shared<Sphere>( -2, 0.25, 0, 0.25 ) );

    // colored balls to see if we are gettingn proper reflections
    auto s = std::make_shared<Sphere>( -1.25, 0.25, 0.75, 0.25 );
    s->material = std::make_shared<DiffuseMaterial>(1, 1, 0);
    container->add( s );
    s = std::make_shared<Sphere>( 1.25, 0.25, 0.75, 0.25 );
    s->material = std::make_shared<DiffuseMaterial>(0, 1, 0);
    container->add( s );
    s = std::make_shared<Sphere>( 0.0, 0.25, 3.0, 0.25 );
    s->material = std::make_shared<DiffuseMaterial>(0, 1, 1);
    container->add( s );

    auto cube = std::make_shared<AxisAlignedSlab>( 0.8, 0, 2.0, 0.5 );
    cube->material = std::make_shared<DiffuseMaterial>( 1.0, 0.2, 0.2 );
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
// Logical Operations
// ------------------------------------------------------------ 
void testLogicalAND()
{
    int imageSize = 256;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight );
    Scene * scene = new Scene();
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    auto obj1 = std::make_shared<Sphere>( -0.5, 1.00, 0, 1.00 );
    auto obj2 = std::make_shared<Sphere>( +0.5, 1.00, 0, 1.00 );
    auto logical = std::make_shared<CSGAnd>( obj1, obj2 );
    //logical->material = std::make_shared<MirrorMaterial>();
    logical->material = std::make_shared<DiffuseMaterial>( 0.5, 1.0, 0.5 );
    //logical->material = std::make_shared<RefractiveMaterial>(1.5);
    container->add( logical );

	scene->root = container;
    tracer.scene = scene;

    //tracer.shader = new AmbientOcclusionShader();
    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_logical_and_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform translation_up = makeTranslation( 0.0, 1.0, 0.0 );
    Transform rotation = makeRotation( -M_PI / 32, Vector4(1, 0, 0) );
    Transform translation_back = makeTranslation( 0.0, 0.0, 10.0 );
    tracer.setCameraTransform( compose( translation_up, rotation, translation_back ) );

    //auto env_map = std::make_shared<ArcLightEnvironmentMap>( Vector4(0, 1, 0), M_PI * 0.4 );
    auto env_map = std::make_shared<ArcLightEnvironmentMap>( Vector4(1, 1, 0), M_PI * 0.4 );
    env_map->setPower( 30.0f );
    scene->env_map = env_map;

    tracer.render();
}

// ------------------------------------------------------------ 
void testLogicalANDMesh()
{
    int imageSize = 256;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 50 );
    Scene * scene = new Scene();
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    std::string modelPath = modelBasePath + "/stanford/bunny/reconstruction";
    auto mesh = loader.load( modelPath + "/bun_zipper_res2.ply" );
    mesh->transform = std::make_shared<Transform>();
    auto bounds = mesh->getAxisAlignedBounds();
    *mesh->transform = compose( makeScaling( 2, 2, 2 ),
                                makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );

    auto obj1 = mesh;
    auto obj2 = std::make_shared<Sphere>( +0.25, 1.00, 0, 1.00 );

    auto logical = std::make_shared<CSGAnd>( obj1, obj2 );
    //logical->material = std::make_shared<MirrorMaterial>();
    logical->material = std::make_shared<DiffuseMaterial>( 0.9, 0.9, 1.0 );
    //logical->material = std::make_shared<RefractiveMaterial>(1.5);
    container->add( logical );
    //container->add( mesh );
    //container->add( obj2 );

	scene->root = container;
    tracer.scene = scene;

    //tracer.shader = new AmbientOcclusionShader();
    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_logical_and_mesh_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform translation_up = makeTranslation( 0.0, 1.0, 0.0 );
    Transform rotation = makeRotation( -M_PI / 32, Vector4(1, 0, 0) );
    Transform rotation_around = makeRotation( -M_PI / 4, Vector4(0, 1, 0) );
    Transform translation_back = makeTranslation( 0.0, 0.0, 10.0 );
    tracer.setCameraTransform( compose( rotation_around, translation_up, rotation, translation_back ) );

    //auto env_map = std::make_shared<ArcLightEnvironmentMap>( Vector4(0, 1, 0), M_PI * 0.4 );
    auto env_map = std::make_shared<ArcLightEnvironmentMap>( Vector4(0.2, 1, 0), M_PI * 0.4 );
    //auto env_map = std::make_shared<ArcLightEnvironmentMap>( Vector4(1, 1, 0), M_PI * 0.4 );
    env_map->setPower( 25.0f );
    scene->env_map = env_map;

    tracer.render();
}
// ------------------------------------------------------------ 
void testLogicalANDLensFocusLight()
{
    int imageSize = 256;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 50 );
    Scene * scene = new Scene();
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    //container->add( floor );

    float lens_thickness = 0.35;
    auto obj1 = std::make_shared<Sphere>( -2.0 + lens_thickness * 0.5, 1.00, 0, 2.00 );
    auto obj2 = std::make_shared<Sphere>( +2.0 - lens_thickness * 0.5, 1.00, 0, 2.00 );
    auto logical = std::make_shared<CSGAnd>( obj1, obj2 );
    logical->material = std::make_shared<RefractiveMaterial>(N_FLINT_GLASS);
    container->add( logical );

    auto cube = std::make_shared<AxisAlignedSlab>( -2.0, 0.0, -2.0,
                                                   -1.0, 2.0, 2.0 );
    cube->material = std::make_shared<DiffuseMaterial>( 1.0, 1.0, 1.0 );
    container->add( cube );

    addSphereLight( container,
                    Vector4( 15.0, 1.0, 0.0 ), 4.5,
                    RGBColor( 1.0, 1.0, 1.0 ), 10.0 );

	scene->root = container;
    tracer.scene = scene;

    //tracer.shader = new AmbientOcclusionShader();
    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_logical_and_focus_light";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation_around = makeRotation( M_PI * 0.2, Vector4(0, 1, 0) );
    //Transform rotation_around = makeRotation( M_PI / 4, Vector4(0, 1, 0) );
    //Transform rotation_around = makeRotation( M_PI / 16, Vector4(0, 1, 0) );
    Transform translation_up = makeTranslation( 0.0, 1.0, 0.0 );
    Transform rotation = makeRotation( -M_PI / 32, Vector4(1, 0, 0) );
    Transform translation_back = makeTranslation( 0.0, 0.0, 10.0 );
    tracer.setCameraTransform( compose( rotation_around, translation_up, rotation, translation_back ) );

    tracer.render();
}

// ------------------------------------------------------------ 
// Base test configuration for a lens made from the logical AND of two spheres
// Derived tests change the index of refraction
BEGIN_SCENE(CSGLogicalANDLens)
    float index_of_refraction = 1.3;
SETUP_SCENE(
    TestScene::setup();
    tracer->rays_per_pixel = 50;
    tracer->shader = new BasicDiffuseSpecularShader();
    tracer->setCameraTransform( compose(
        // move up a bit
        makeTranslation( 0.0, 1.0, 0.0 ),
        // rotate so we are looking down
        makeRotation( -M_PI / 32, Vector4(1, 0, 0) ),
        // back away from the origin
        makeTranslation( 0.0, 0.0, 10.0 )
        ) );
);
std::shared_ptr<TriangleMesh> mesh = nullptr;
BUILD_SCENE(
    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    auto obj1 = std::make_shared<Sphere>( -9.95, 1.00, 0, 10.00 );
    auto obj2 = std::make_shared<Sphere>( +9.95, 1.00, 0, 10.00 );
    auto logical = std::make_shared<CSGAnd>( obj1, obj2 );
    logical->material = std::make_shared<RefractiveMaterial>(index_of_refraction);
    logical->transform = std::make_shared<Transform>();
    *logical->transform = makeRotation( -M_PI / 2, Vector4(0, 1, 0) );
    container->add( logical );

    // Colored strips to show refraction from background objects
    auto cube = std::make_shared<AxisAlignedSlab>( -10.0, 0.0, -5.0, +10.0, 0.15, -5.15 );
    cube->material = std::make_shared<DiffuseMaterial>( 0.5, 0.5, 1.0 );
    container->add( cube );
    cube = std::make_shared<AxisAlignedSlab>( -10.0, 0.5 + 0.0, -5.0, +10.0, 0.5 + 0.15, -5.15 );
    cube->material = std::make_shared<DiffuseMaterial>( 1.0, 0.5, 0.0 );
    container->add( cube );
    cube = std::make_shared<AxisAlignedSlab>( -10.0, 1.0 + 0.0, -5.0, +10.0, 1.0 + 0.15, -5.15 );
    cube->material = std::make_shared<DiffuseMaterial>( 0.0, 1.0, 0.5 );
    container->add( cube );

    //auto env_map = std::make_shared<ArcLightEnvironmentMap>( Vector4(1, 1, 1), M_PI * 0.4 );
    //env_map->setPower( 30.0f );
    //scene->env_map = env_map;
    scene->env_map = std::make_shared<TestPatternEnvironmentMap>();
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(CSGLogicalANDLensWater, CSGLogicalANDLens)
SETUP_SCENE(
    CSGLogicalANDLens::setup();
    index_of_refraction = N_WATER;
);
BUILD_SCENE(
    CSGLogicalANDLens::buildScene();
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(CSGLogicalANDLensCrownGlass, CSGLogicalANDLens)
SETUP_SCENE(
    CSGLogicalANDLens::setup();
    index_of_refraction = N_CROWN_GLASS;
);
BUILD_SCENE(
    CSGLogicalANDLens::buildScene();
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(CSGLogicalANDLensFlintGlass, CSGLogicalANDLens)
SETUP_SCENE(
    CSGLogicalANDLens::setup();
    index_of_refraction = N_FLINT_GLASS;
);
BUILD_SCENE(
    CSGLogicalANDLens::buildScene();
);
END_SCENE()

// ------------------------------------------------------------ 
// ------------------------------------------------------------ 
// Reflection
// ------------------------------------------------------------ 
void testReflection1()
{
    int imageSize = 320;
    int imageWidth = imageSize, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 100 );
    Scene * scene = new Scene();
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    std::shared_ptr<Sphere> sphere;

    sphere = std::make_shared<Sphere>( -2, 0.25, 0.5, 0.25 );
    sphere->material = std::make_shared<DiffuseMaterial>( 1.0, 0.5, 0.5 );
    container->add( sphere );

    sphere = std::make_shared<Sphere>( 0, 0.50, 1.5, 0.50 );
    sphere->material = std::make_shared<DiffuseMaterial>( 0.5, 1.0, 0.5 );
    container->add( sphere );

    sphere = std::make_shared<Sphere>( -1, 0.75, 0, 0.75  );
    sphere->material = std::make_shared<MirrorMaterial>();
    container->add( sphere );

    sphere = std::make_shared<Sphere>( +1, 1.00, 0, 1.00 );
    sphere->material = std::make_shared<DiffuseMaterial>( 0.5, 0.5, 1.0 );
    container->add( sphere );

	scene->root = container;
    scene->env_map = std::make_shared<ArcLightEnvironmentMap>();
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    std::shared_ptr<Sphere> sphere = nullptr;

    sphere = std::make_shared<Sphere>( -2, 0.25, 0.5, 0.25 );
    sphere->material = std::make_shared<DiffuseMaterial>( 1.0, 0.5, 0.5 );
    container->add( sphere );

    sphere = std::make_shared<Sphere>( +1, 0.50, 4.0, 0.50 );
    sphere->material = std::make_shared<DiffuseMaterial>( 0.5, 1.0, 0.5 );
    container->add( sphere );

    sphere = std::make_shared<Sphere>( -1, 0.75, 0, 0.75  );
    sphere->material = std::make_shared<MirrorMaterial>( 0.75, 0.75, 0.15 );
    container->add( sphere );

    sphere = std::make_shared<Sphere>( +1, 1.00, 0, 1.00 );
    sphere->material = std::make_shared<DiffuseMaterial>( 0.5, 0.5, 1.0 );
    container->add( sphere );

    sphere = std::make_shared<Sphere>( 0, 0.5, 2.0, 0.5  );
    sphere->material = std::make_shared<MirrorMaterial>();
    container->add( sphere );

    sphere = std::make_shared<Sphere>( -0.75, 0.25, 3.5, 0.25  );
    sphere->material = std::make_shared<MirrorMaterial>( 1.0, 0.0, 1.0 );
    container->add( sphere );

	scene->root = container;
    scene->env_map = std::make_shared<ArcLightEnvironmentMap>();
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    std::string modelPath = modelBasePath + "/stanford/bunny/reconstruction";
    auto mesh = loader.load( modelPath + "/bun_zipper_res2.ply" );
    auto bounds = mesh->getAxisAlignedBounds();

    //mesh->material = std::make_shared<DiffuseMaterial>( 0.75, 1.0, 0.8 );
    mesh->material = std::make_shared<MirrorMaterial>();

    mesh->transform = std::make_shared<Transform>();
    *mesh->transform = compose( makeScaling( 2, 2, 2 ),
                                makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
    container->add( mesh );

    std::shared_ptr<Sphere> sphere = nullptr;

    sphere = std::make_shared<Sphere>( -2, 0.25, 2.5, 0.25 );
    sphere->material = std::make_shared<DiffuseMaterial>( 1.0, 0.5, 0.5 );
    container->add( sphere );

    sphere = std::make_shared<Sphere>( -2, 0.75, 0, 0.75 );
    sphere->material = std::make_shared<MirrorMaterial>();
    container->add( sphere );

    sphere = std::make_shared<Sphere>( +2.5, 1.00, 0, 1.00 );
    sphere->material = std::make_shared<DiffuseMaterial>( 0.5, 0.5, 1.0 );
    container->add( sphere );

    sphere = std::make_shared<Sphere>( -0.75, 0.50, 4.0, 0.50 );
    sphere->material = std::make_shared<MirrorMaterial>( 1.0, 0.2, 1.0 );
    container->add( sphere );

    std::shared_ptr<AxisAlignedSlab> cube;

    cube = std::make_shared<AxisAlignedSlab>( +0.50, 0, 5.0, 1.0 );
    cube->material = std::make_shared<DiffuseMaterial>( 0.5, 1.0, 0.5 );
    container->add( cube );

	scene->root = container;
    scene->env_map = std::make_shared<ArcLightEnvironmentMap>();
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    std::shared_ptr<Sphere> sphere;

    //sphere = std::make_shared<Sphere>( -2, 0.25, 0.5, 0.25 );
    //sphere->material = std::make_shared<DiffuseMaterial>( 1.0, 0.5, 0.5 );
    //container->add( sphere );

    sphere = std::make_shared<Sphere>( 0, 0.50, 1.5, 0.50 );
    sphere->material = std::make_shared<RefractiveMaterial>(1.2);
    //sphere->material = std::make_shared<RefractiveMaterial>(1.5);
    container->add( sphere );

    sphere = std::make_shared<Sphere>( -1, 1.50, 10.0, 0.50 );
    //sphere->material = std::make_shared<RefractiveMaterial>(1.2);
    sphere->material = std::make_shared<RefractiveMaterial>(1.5);
    container->add( sphere );

    sphere = std::make_shared<Sphere>( -1, 0.75, 0, 0.75  );
    sphere->material = std::make_shared<MirrorMaterial>();
    container->add( sphere );

    //sphere = std::make_shared<Sphere>( +1, 1.00, 0, 1.00 );
    //sphere->material = std::make_shared<DiffuseMaterial>( 0.5, 0.5, 1.0 );
    //container->add( sphere );

    auto cube = std::make_shared<AxisAlignedSlab>( -0.75, -0.75, -0.75, 1.5 );
    cube->material = std::make_shared<RefractiveMaterial>(1.2);
    container->add( cube );
    cube->transform = std::make_shared<TimeVaryingTransform>(
        [](float anim_progress) {
            return compose( makeTranslation( Vector4( 1.75, 1.2, 2.75 ) ),
                            //makeRotation( anim_progress * 0.5 * M_PI + 0.6, Vector4(0, 1, 0) ) );
                            makeRotation( anim_progress * 0.5 * M_PI, Vector4(0, 1, 0) ) );
                            
        });

#if 0
    cube = std::make_shared<AxisAlignedSlab>( -0.50, -0.50, -0.50, 1.0 );
    cube->material = std::make_shared<RefractiveMaterial>(1.2);
    container->add( cube );
    cube->transform = std::make_shared<TimeVaryingTransform>(
        [](float anim_progress) {
            return compose( makeTranslation( Vector4( -1.50, 1.00, 6.50 ) ),
                            makeRotation( anim_progress * 0.5 * M_PI, Vector4(1, 0, 0) ) );
                            
        });
#endif

    // Colored strips to show refraction from background objects
    cube = std::make_shared<AxisAlignedSlab>( -10.0, 0.0, -2.0,
                                +10.0, 0.15, -2.15 );
    cube->material = std::make_shared<DiffuseMaterial>( 0.5, 0.5, 1.0 );
    container->add( cube );

    cube = std::make_shared<AxisAlignedSlab>( -10.0, 0.0, -0.0,
                                +10.0, 0.15, -0.15 );
    cube->material = std::make_shared<DiffuseMaterial>( 1.0, 0.5, 0.0 );
    container->add( cube );

    cube = std::make_shared<AxisAlignedSlab>( -10.0, 0.0, 2.0,
                                +10.0, 0.15, 2.15 );
    cube->material = std::make_shared<DiffuseMaterial>( 0.0, 1.0, 0.5 );
    container->add( cube );
	scene->root = container;

    scene->env_map = std::make_shared<ArcLightEnvironmentMap>(Vector4(0, 1, 0), 0.25 * M_PI);
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    std::shared_ptr<TriangleMesh> mesh;

    Transform rotation = makeRotation( M_PI / 4.0, Vector4(0, 1, 0) );
    float spacing = 2.0;

    // high res
    std::string model = "models/stanford/bunny/reconstruction/bun_zipper.ply";
    // low res
    //std::string model = "models/stanford/bunny/reconstruction/bun_zipper_res4.ply";

    {
        mesh = loader.load( model );
        auto bounds = mesh->getAxisAlignedBounds();

        mesh->material = std::make_shared<RefractiveMaterial>(N_AIR);

        mesh->transform = std::make_shared<Transform>();
        *mesh->transform = compose( makeTranslation( Vector4( -2.0 * spacing, 0.1, 2.0 ) ),
                                    makeScaling( 3, 3, 3 ),
                                    rotation,
                                    makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
        container->add( mesh );
    }

    {
        mesh = loader.load( model );
        auto bounds = mesh->getAxisAlignedBounds();

        mesh->material = std::make_shared<RefractiveMaterial>(N_WATER);

        mesh->transform = std::make_shared<Transform>();
        *mesh->transform = compose( makeTranslation( Vector4( -1.0 * spacing, 0.1, 2.0 ) ),
                                    makeScaling( 3, 3, 3 ),
                                    rotation,
                                    makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
        container->add( mesh );
    }

    {
        mesh = loader.load( model );
        auto bounds = mesh->getAxisAlignedBounds();

        mesh->material = std::make_shared<RefractiveMaterial>(N_PLEXIGLAS);

        mesh->transform = std::make_shared<Transform>();
        *mesh->transform = compose( makeTranslation( Vector4( 0.0, 0.1, 2.0 ) ),
                                    makeScaling( 3, 3, 3 ),
                                    rotation,
                                    makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
        container->add( mesh );
    }

    {
        mesh = loader.load( model );
        auto bounds = mesh->getAxisAlignedBounds();

        mesh->material = std::make_shared<RefractiveMaterial>(N_FLINT_GLASS);

        mesh->transform = std::make_shared<Transform>();
        *mesh->transform = compose( makeTranslation( Vector4( 1.0 * spacing, 0.1, 2.0 ) ),
                                    makeScaling( 3, 3, 3 ),
                                    rotation,
                                    makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
        container->add( mesh );
    }

    {
        mesh = loader.load( model );
        auto bounds = mesh->getAxisAlignedBounds();

        mesh->material = std::make_shared<RefractiveMaterial>(N_DIAMOND);

        mesh->transform = std::make_shared<Transform>();
        *mesh->transform = compose( makeTranslation( Vector4( 2.0 * spacing, 0.1, 2.0 ) ),
                                    makeScaling( 3, 3, 3 ),
                                    rotation,
                                    makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
        container->add( mesh );
    }

    // Walls
#if 1
    auto wall = std::make_shared<AxisAlignedSlab>( -10.0, 0.0, -10.0,
                                     +10.0, 10.0, -10.0 );
    wall->material = std::make_shared<DiffuseMaterial>( 0.8, 0.8, 1.0 );
    container->add( wall );
#endif

    // Colored strips to show refraction from background objects
    auto cube = std::make_shared<AxisAlignedSlab>( -10.0, 0.0, -2.0,
                                     +10.0, 0.15, -2.15 );
    cube->material = std::make_shared<DiffuseMaterial>( 0.5, 0.5, 1.0 );
    container->add( cube );

    cube = std::make_shared<AxisAlignedSlab>( -10.0, 0.5 + 0.0, -2.0,
                                +10.0, 0.5 + 0.15, -2.15 );
    cube->material = std::make_shared<DiffuseMaterial>( 1.0, 0.5, 0.0 );
    container->add( cube );

    cube = std::make_shared<AxisAlignedSlab>( -10.0, 1.0 + 0.0, -2.0,
                                +10.0, 1.0 + 0.15, -2.15 );
    cube->material = std::make_shared<DiffuseMaterial>( 0.0, 1.0, 0.5 );
    container->add( cube );
	scene->root = container;

	scene->root = container;
    scene->env_map = std::make_shared<ArcLightEnvironmentMap>();
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    std::shared_ptr<Sphere> sphere;
    float y = 1.5, z = 2.0, radius = 1.0;

    sphere = std::make_shared<Sphere>( -4, y, z, radius );
    sphere->material = std::make_shared<RefractiveMaterial>(1.3);
    container->add( sphere );

    sphere = std::make_shared<Sphere>( -2, y, z, radius );
    sphere->material = std::make_shared<RefractiveMaterial>(1.49);
    container->add( sphere );

    sphere = std::make_shared<Sphere>( 0, y, z, radius );
    sphere->material = std::make_shared<RefractiveMaterial>(1.6);
    container->add( sphere );

    sphere = std::make_shared<Sphere>( +2, y, z, radius );
    sphere->material = std::make_shared<RefractiveMaterial>(2.1);
    container->add( sphere );

    sphere = std::make_shared<Sphere>( +4, y, z, radius );
    sphere->material = std::make_shared<RefractiveMaterial>(2.5);
    container->add( sphere );

    // Colored strips to show refraction from background objects
    auto cube = std::make_shared<AxisAlignedSlab>( -10.0, 0.0, -2.0,
                                     +10.0, 0.15, -2.15 );
    cube->material = std::make_shared<DiffuseMaterial>( 0.5, 0.5, 1.0 );
    container->add( cube );

    cube = std::make_shared<AxisAlignedSlab>( -10.0, 0.5 + 0.0, -2.0,
                                +10.0, 0.5 + 0.15, -2.15 );
    cube->material = std::make_shared<DiffuseMaterial>( 1.0, 0.5, 0.0 );
    container->add( cube );

    cube = std::make_shared<AxisAlignedSlab>( -10.0, 1.0 + 0.0, -2.0,
                                +10.0, 1.0 + 0.15, -2.15 );
    cube->material = std::make_shared<DiffuseMaterial>( 0.0, 1.0, 0.5 );
    container->add( cube );
	scene->root = container;

	scene->root = container;
    scene->env_map = std::make_shared<ArcLightEnvironmentMap>(Vector4(0, 1, 0), M_PI * 0.25);
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, 0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    auto sphere = std::make_shared<Sphere>( 0.0, 1.25, 0.0, 1.0 );
    sphere->material = std::make_shared<RefractiveMaterial>(1.4);
    container->add( sphere );

    auto foo = std::make_shared<AxisAlignedSlab>( -0.5, -0.5, -0.5, 1.0 );
    foo->material = std::make_shared<RefractiveMaterial>(1.1);
    foo->transform = std::make_shared<Transform>();
    *foo->transform = compose( makeTranslation( Vector4( 1.5, 0.75, 0.0 ) ),
                                //makeRotation( -0.8 * M_PI, Vector4(0, 1, 0) ) );
                                makeRotation( 0.0, Vector4(0, 1, 0) ) );
    container->add( foo );

    // Light
    auto cube = std::make_shared<AxisAlignedSlab>( 2.0, 2.5, -1.0,
                                     4.0, 4.5,  1.0 );
    cube->material = std::make_shared<Material>();
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    std::shared_ptr<TriangleMesh> mesh;

    {
        mesh = loader.load( "models/stanford/bunny/reconstruction/bun_zipper.ply" );
        auto bounds = mesh->getAxisAlignedBounds();

        //mesh->material = std::make_shared<DiffuseMaterial>( 0.75, 1.0, 0.8 );
        //mesh->material = std::make_shared<MirrorMaterial>();

        mesh->transform = std::make_shared<Transform>();
        *mesh->transform = compose( makeScaling( 2, 2, 2 ),
                                    makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
        container->add( mesh );
    }

    {
        mesh = loader.load( "models/stanford/dragon/reconstruction/dragon_vrip.ply" );
        auto bounds = mesh->getAxisAlignedBounds();

        mesh->transform = std::make_shared<Transform>();
        *mesh->transform = compose( makeTranslation( 3.0, 0.0, 0.0 ),
                                    compose( makeScaling( 4, 4, 4 ),
                                             makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) ) );;
        container->add( mesh );
    }

	scene->root = container;
    scene->env_map = std::make_shared<ArcLightEnvironmentMap>();
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    auto meshes = loader.loadMultiPart( "models/nasa/lunarlandernofoil-c/lunarlandernofoil_carbajal.3ds" );
    //auto meshes = loader.loadMultiPartMerged( "models/nasa/lunarlandernofoil-c/lunarlandernofoil_carbajal.3ds" );
    meshes->transform = std::make_shared<Transform>();
    *meshes->transform = makeRotation( -0.5 * M_PI, Vector4( 1.0, 0.0, 0.0 ) );
    container->add( meshes );

	scene->root = container;
    //scene->env_map = std::make_shared<ArcLightEnvironmentMap>();
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
    int imageSize = 50;
    //int imageSize = 100;
    //int imageSize = 256;
    //int imageSize = 320;
    //int imageSize = 512;
    //int imageSize = 1024;
    int imageWidth = imageSize, imageHeight = imageSize;
    //ImageTracer tracer( imageWidth, imageHeight, 1, 10 );
    ImageTracer tracer( imageWidth, imageHeight, 1, 1 );
    Scene * scene = new Scene();
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    //auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
    //                                               +10.0, -1.0, -10.0 );
    //container->add( floor );

    auto mesh = loader.loadMultiPart( "models/san-miguel/san-miguel.obj" );
    //auto mesh = loader.loadMultiPartMerged( "models/san-miguel/san-miguel.obj" );
    //auto bounds = mesh->getAxisAlignedBounds();
    mesh->transform = std::make_shared<Transform>();
    //float scale = 35.0;
    float scale = 1.0;
    *mesh->transform = compose( makeScaling( scale, scale, scale ),
                                makeRotation( M_PI / 2, Vector4( 0.0, 1.0, 0.0 ) )//,
                              );
                               // makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
    container->add( mesh );

    auto s = std::make_shared<Sphere>( -1.25, 0.25, 0.75, 0.25 );
    s->material = std::make_shared<DiffuseMaterial>(1, 1, 0);
    container->add( s );
    s = std::make_shared<Sphere>( 1.25, 0.25, 0.75, 0.25 );
    s->material = std::make_shared<DiffuseMaterial>(0, 1, 0);
    container->add( s );
    s = std::make_shared<Sphere>( 0.0, 0.25, 3.0, 0.25 );
    s->material = std::make_shared<DiffuseMaterial>(0, 1, 1);
    container->add( s );

    scene->addPointLight( PointLight( Vector4( 20.0, 20.0, -15.0 ),
        RGBColor( 1.0, 1.0, 1.0 ).scaled(150.0) ) );
    scene->addPointLight( PointLight( Vector4( 20.0, 20.0, 10.0 ),
        RGBColor( 1.0, 1.0, 1.0 ).scaled(1300.0) ) );
    scene->addPointLight( PointLight( Vector4( 0.0, 20.0, 5.0 ),
        RGBColor( 1.0, 1.0, 1.0 ).scaled(500.0) ) );

	scene->root = container;
    //scene->env_map = std::make_shared<ArcLightEnvironmentMap>();
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

// Big multipart mesh
void testMeshDabrovicSponza()
{
    //int imageSize = 50;
    //int imageSize = 100;
    int imageSize = 256;
    //int imageSize = 320;
    //int imageSize = 512;
    //int imageSize = 1024;
    int imageWidth = imageSize, imageHeight = imageSize;
    //ImageTracer tracer( imageWidth, imageHeight, 1, 100 );
    //ImageTracer tracer( imageWidth, imageHeight, 1, 30 );
    ImageTracer tracer( imageWidth, imageHeight, 1, 10 );
    //ImageTracer tracer( imageWidth, imageHeight, 1, 3 );
    //ImageTracer tracer( imageWidth, imageHeight, 1, 1 );
    tracer.camera.xmin = -0.45;
    tracer.camera.xmax = 0.45;
    tracer.camera.ymin = -0.45;
    tracer.camera.ymax = 0.45;
    //tracer.min_flush_period_seconds = 100000.0;
    Scene * scene = new Scene();
	auto container = std::make_shared<FlatContainer>();

    auto mesh = loader.loadMultiPart( "models/dabrovic-sponza/sponza.obj" );
    if( !mesh ) { fprintf( stderr, "Error loading meshes\n" ); return; }
    //auto bounds = mesh->getAxisAlignedBounds();
    mesh->transform = std::make_shared<Transform>();
    float scale = 1.0;
    *mesh->transform = compose( makeScaling( scale, scale, scale ),
                                makeRotation( M_PI / 2, Vector4( 0.0, 1.0, 0.0 ) )//,
                              );
    container->add( mesh );

    scene->addPointLight( PointLight( Vector4( 3.0, 1.0, 0.0 ),
                                      RGBColor( 1.0, 0.5, 0.3 ).scaled(20.0) ) );

    //addSphereLight( container,
    //                Vector4( 0.0, 10.0, 0.0 ), 1.5,
    //                RGBColor( 0.5, 0.5, 1.0 ), 5.0 );
    auto emitter = std::make_shared<AxisAlignedSlab>( -1.5, 15.0, -20.0,
                                                       1.5, 16.0,  20.0 );
    emitter->material = std::make_shared<Material>();
    emitter->material->emittance = RGBColor( 0.5, 0.5, 1.0 );
    emitter->material->emittance.scale( 5.0 );
    container->add( emitter );

	scene->root = container;
    tracer.scene = scene;
    //auto env_map = std::make_shared<ArcLightEnvironmentMap>( Vector4(0, 1, 0), M_PI / 4.0 );
    //env_map->setPower( 1000.0f );
    //scene->env_map = env_map;


    tracer.shader = new BasicDiffuseSpecularShader();
    //tracer.shader = new AmbientOcclusionShader();
    //tracer.shader = new GoochShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_mesh_dabrovic_sponza_";

#if 1
    tracer.setCameraTransform( compose( makeTranslation( 0.0, 5.0, 10.0 ),
                                        makeRotation( 0.0 * M_PI, Vector4(1, 0, 0) ),
                                        makeTranslation( 0.0, 0.0, 0.0 ) ) );
#else
    tracer.setCameraTransform( compose( makeTranslation( 0.0, -10.0, -50.0 ),
                                        makeRotation( -0.11 * M_PI, Vector4(1, 0, 0) ),
                                        makeTranslation( 0.0, 0.0, 50.0 ) ) );
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    std::shared_ptr<TriangleMesh> mesh;

    {
        mesh = loader.load( "models/hairball.obj" );
        auto bounds = mesh->getAxisAlignedBounds();

        //mesh->material = std::make_shared<DiffuseMaterial>( 0.75, 1.0, 0.8 );
        //mesh->material = std::make_shared<MirrorMaterial>();

        mesh->transform = std::make_shared<Transform>();
        *mesh->transform = compose( makeScaling( 2, 2, 2 ),
                                    makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
        container->add( mesh );
    }

	scene->root = container;
    //scene->env_map = std::make_shared<ArcLightEnvironmentMap>();
    scene->env_map = std::make_shared<ArcLightEnvironmentMap>(Vector4(1,1,0), M_PI / 4.0f);
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane at y=0
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, +0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    std::string modelPath = modelBasePath + "/stanford/bunny/reconstruction";
    auto mesh = loader.load( modelPath + "/bun_zipper_res2.ply" );
    auto bounds = mesh->getAxisAlignedBounds();

    mesh->material = std::make_shared<MirrorMaterial>();

    float ymin = bounds->ymin;
    mesh->transform = std::make_shared<TimeVaryingTransform>(
        [ymin](float anim_progress) {
            return compose( makeScaling( 2, 2, 2 ),
                            makeRotation( anim_progress * 2.0 * M_PI, Vector4(0, 1, 0) ),
                            makeTranslation( Vector4( 0.0, -ymin, 0.0 ) ) );
        });
    container->add( mesh );

    container->add( std::make_shared<Sphere>( -2, 0.25, 0, 0.25 ) );

    // colored balls to see if we are gettingn proper reflections
    auto s = std::make_shared<Sphere>( -1.25, 0.25, 0.75, 0.25 );
    s->material = std::make_shared<DiffuseMaterial>(1, 1, 0);
    container->add( s );
    s = std::make_shared<Sphere>( 1.25, 0.25, 0.75, 0.25 );
    s->material = std::make_shared<DiffuseMaterial>(0, 1, 0);
    container->add( s );
    s = std::make_shared<Sphere>( 0.0, 0.25, 3.0, 0.25 );
    s->material = std::make_shared<DiffuseMaterial>(0, 1, 1);
    container->add( s );

    auto cube = std::make_shared<AxisAlignedSlab>( 0.8, 0, 2.0, 0.5 );
    cube->material = std::make_shared<DiffuseMaterial>( 1.0, 0.2, 0.2 );
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, -2.0, +10.0,
                                                   +10.0, -3.0, -10.0 );
    container->add( floor );

    auto cube = std::make_shared<AxisAlignedSlab>( -0.5, -0.5, -0.5, 1.0 );
    cube->transform = std::make_shared<TimeVaryingTransform>(
        [](float anim_progress) {
            return compose( makeTranslation( Vector4( -2.0, 0.0, 0.0 ) ),
                            makeRotation( (anim_progress - 0.4) * 2.0 * M_PI, Vector4(1, 0, 0) ) );
        });
    container->add( cube );
    cube = std::make_shared<AxisAlignedSlab>( -0.5, -0.5, -0.5, 1.0 );
    cube->transform = std::make_shared<TimeVaryingTransform>(
        [](float anim_progress) {
            return compose( makeTranslation( Vector4( 0.0, 0.0, 0.0 ) ),
                            makeRotation( (anim_progress - 0.4) * 2.0 * M_PI, Vector4(0, 1, 0) ) );
        });
    container->add( cube );
    cube = std::make_shared<AxisAlignedSlab>( -0.5, -0.5, -0.5, 1.0 );
    cube->transform = std::make_shared<TimeVaryingTransform>(
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, -2.0, +10.0,
                                                   +10.0, -3.0, -10.0 );
    container->add( floor );

    auto cube = std::make_shared<AxisAlignedSlab>( -0.5, -0.5, -0.5, 1.0 );
    cube->material = std::make_shared<MirrorMaterial>();
    cube->transform = std::make_shared<TimeVaryingTransform>(
        [](float anim_progress) {
            return compose( makeTranslation( Vector4( -2.0, 0.0, 0.0 ) ),
                            makeRotation( (anim_progress - 0.4) * 0.5 * M_PI, Vector4(1, 0, 0) ) );
        });
    container->add( cube );
    cube = std::make_shared<AxisAlignedSlab>( -0.5, -0.5, -0.5, 1.0 );
    cube->material = std::make_shared<MirrorMaterial>();
    cube->transform = std::make_shared<TimeVaryingTransform>(
        [](float anim_progress) {
            return compose( makeTranslation( Vector4( 0.0, 0.0, 0.0 ) ),
                            makeRotation( (anim_progress - 0.4) * 0.5 * M_PI, Vector4(0, 1, 0) ) );
        });
    container->add( cube );
    cube = std::make_shared<AxisAlignedSlab>( -0.5, -0.5, -0.5, 1.0 );
    cube->material = std::make_shared<MirrorMaterial>();
    cube->transform = std::make_shared<TimeVaryingTransform>(
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, 0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    auto cube = std::make_shared<AxisAlignedSlab>( -0.5, -0.5, -0.5, 1.0 );
    //cube->material = std::make_shared<MirrorMaterial>();
    cube->transform = std::make_shared<Transform>();
    *cube->transform = compose( makeTranslation( Vector4( 0.0, 0.5, 0.0 ) ),
                                makeRotation( -0.8 * M_PI, Vector4(0, 1, 0) ) );
    container->add( cube );

    // Light
    cube = std::make_shared<AxisAlignedSlab>( 2.0, 2.5, -2.25,
                                              3.0, 3.5,  2.25 );
    cube->material = std::make_shared<Material>();
    cube->material->emittance = RGBColor( 1.0, 1.0, 1.0 );
    cube->material->emittance.scale( 5.0 );
    container->add( cube );

    auto env_map = std::make_shared<ArcLightEnvironmentMap>( Vector4(0, 1, 0), M_PI / 2.0 );
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
	auto container = std::make_shared<FlatContainer>();

    // Ground plane
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, 0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    std::shared_ptr<AxisAlignedSlab> cube;

#if 0
    cube = std::make_shared<AxisAlignedSlab>( -0.5, -0.5, -0.5, 1.0 );
    //cube->material = std::make_shared<MirrorMaterial>();
    cube->transform = std::make_shared<Transform>();
    *cube->transform = compose( makeTranslation( Vector4( -0.8, 0.5, 0.0 ) ),
                                makeRotation( -0.1 * M_PI, Vector4(0, 1, 0) ) );
    container->add( cube );
#endif

    int num_cubes = 20;
    float min_angle = -M_PI / 2.0;
    float max_angle = M_PI / 2.0;
    for( int i = 0; i < num_cubes; i++ ) {
        float angle = (float) i / (num_cubes-1) * (max_angle - min_angle) + min_angle;
        cube = std::make_shared<AxisAlignedSlab>( -0.5, -0.5, -0.5, 1.0 );
        cube->material = std::make_shared<MirrorMaterial>();
        cube->transform = std::make_shared<Transform>();
        *cube->transform = compose( makeRotation( angle, Vector4(0, 1, 0) ),
                                    makeTranslation( Vector4( -2.0, 0.5, 0.0 ) ),
                                    makeScaling( 0.1, 1.0, 0.4 ) );
        container->add( cube );
    }

#if 0
    cube = std::make_shared<AxisAlignedSlab>( -0.5, -0.5, -0.5, 1.0 );
    cube->transform = std::make_shared<Transform>();
    *cube->transform = compose( makeTranslation( Vector4( 0.8, 0.5, 0.0 ) ),
                                makeRotation( -0.8 * M_PI, Vector4(0, 1, 0) ) );
    container->add( cube );
#endif


    // Light
    cube = std::make_shared<AxisAlignedSlab>( 2.0, 2.5, -2.25,
                                3.0, 3.5,  2.25 );
    cube->material = std::make_shared<Material>();
    cube->material->emittance = RGBColor( 1.0, 1.0, 1.0 );
    cube->material->emittance.scale( 5.0 );
    container->add( cube );

    //auto env_map = std::make_shared<ArcLightEnvironmentMap>( Vector4(0, 1, 0), M_PI / 2.0 );
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

// Test with a real circular area light
void testCircleAreaLight1()
{
    //int imageSize = 512;
    int imageSize = 256;
    //int imageSize = 128;
    int imageWidth = imageSize, imageHeight = imageSize;
    int rays_per_pixel = 30;
    ImageTracer tracer( imageWidth, imageHeight, 1, rays_per_pixel );
    Scene * scene = new Scene();
	auto container = std::make_shared<FlatContainer>();

    // Ground plane
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, 0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    auto cube = std::make_shared<AxisAlignedSlab>( -0.5, -0.5, -0.5, 1.0 );
    cube->transform = std::make_shared<Transform>();
    *cube->transform = compose( makeTranslation( Vector4( 0.0, 0.5, 0.0 ) ),
                                makeRotation( -0.8 * M_PI, Vector4(0, 1, 0) ) );
    container->add( cube );

    float sradius = 0.5;
    auto sphere = std::make_shared<Sphere>( 0.0, sradius, 0.0, sradius );
    //container->add( sphere );

    std::string modelPath = modelBasePath + "/stanford/bunny/reconstruction";
    auto mesh = loader.load( modelPath + "/bun_zipper_res2.ply" );
    auto bounds = mesh->getAxisAlignedBounds();

    mesh->transform = std::make_shared<Transform>();
    *mesh->transform = compose( makeScaling( 1.5, 1.5, 1.5 ),
                                makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
    //container->add( mesh );

    // Area Light
    auto light_color = RGBColor( 1.0, 1.0, 1.0 ).scaled( 5.0 );
    auto light = std::make_shared<CircleAreaLight>( 2.0, light_color );
    light->transform = std::make_shared<Transform>();
    *light->transform = compose( makeRotation( M_PI * 0.0, Vector4(0, 0, 1) ),
                                 makeTranslation( Vector4( 0, 3.0, 0 ) ) );
    container->add( light );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_circle_area_light1_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 18.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

void testAnimLights1()
{
    int imageSize = 256;
    int imageWidth = imageSize, imageHeight = imageSize;
    int rays_per_pixel = 100;
    int num_frames = 15;
    ImageTracer tracer( imageWidth, imageHeight, num_frames, rays_per_pixel );
    Scene * scene = new Scene();
	auto container = std::make_shared<FlatContainer>();

    // Ground plane
    auto floor = std::make_shared<AxisAlignedSlab>( -10.0, 0.0, +10.0,
                                                   +10.0, -1.0, -10.0 );
    container->add( floor );

    auto cube = std::make_shared<AxisAlignedSlab>( -0.5, -0.5, -0.5, 1.0 );
    cube->transform = std::make_shared<Transform>();
    *cube->transform = compose( makeTranslation( Vector4( 0.0, 0.5, 0.0 ) ),
                                makeRotation( -0.8 * M_PI, Vector4(0, 1, 0) ) );
    container->add( cube );

    float sradius = 0.5;
    auto sphere = std::make_shared<Sphere>( 0.0, sradius, 0.0, sradius );
    //container->add( sphere );

    std::string modelPath = modelBasePath + "/stanford/bunny/reconstruction";
    auto mesh = loader.load( modelPath + "/bun_zipper_res2.ply" );
    auto bounds = mesh->getAxisAlignedBounds();

    mesh->transform = std::make_shared<Transform>();
    *mesh->transform = compose( makeScaling( 1.5, 1.5, 1.5 ),
                                makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
    //container->add( mesh );

    // Area Light
    auto light_color = RGBColor( 1.0, 1.0, 1.0 ).scaled( 10.0 );
    auto light = std::make_shared<CircleAreaLight>( 1.0, light_color );
#if 1
    light->transform = std::make_shared<TimeVaryingTransform>(
        [](float anim_progress) {
            return compose( makeRotation( anim_progress * 2.0 * M_PI, Vector4(0, 1, 0) ),
                            makeRotation( M_PI * 0.25, Vector4(0, 0, 1) ),
                            makeTranslation( Vector4( 0, 3.0, 0 ) ) );
        });
#else
    light->transform = std::make_shared<Transform>();
    *light->transform = compose( makeRotation( M_PI * 0.0, Vector4(0, 0, 1) ),
                                 makeTranslation( Vector4( 0, 3.0, 0 ) ) );
#endif
    container->add( light );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_anim_lights1_";

    // Camera back and rotated a bit around x so we're looking slightly down
    Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
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
    auto cube = std::make_shared<AxisAlignedSlab>( -half_size, 0.0, -half_size,
                                     half_size, size, half_size );
    cube->material = std::make_shared<DiffuseMaterial>( 0.5, 0.5, 1.0 );
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
    auto cube = std::make_shared<AxisAlignedSlab>( -half_size, 0.0, -half_size,
                                                    half_size, size, half_size );
    cube->transform = std::make_shared<Transform>();
    *cube->transform = makeTranslation( Vector4( -1.0, 0.0, 0.0 ) );
    container->add( cube );

    auto sphere = std::make_shared<Sphere>( 1.0, half_size, 0.0, half_size );
    container->add( sphere );

    auto mesh = loader.load( modelBasePath + "/stanford/happy/reconstruction/happy_vrip_res4.ply" );
    auto bounds = mesh->getAxisAlignedBounds();

    mesh->transform = std::make_shared<Transform>();
    //*mesh->transform = makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) );
    *mesh->transform = makeScaling( 3.0 );
    container->add( mesh );
);
END_SCENE()
// ------------------------------------------------------------ 
BEGIN_SCENE( RoomScene )
SETUP_SCENE( TestScene::setup(); );
BUILD_SCENE(
    auto white_material = std::make_shared<DiffuseMaterial>( 1.0, 1.0, 1.0 );
    auto red_material = std::make_shared<DiffuseMaterial>( 1.0, 0.5, 0.5 );
    auto blue_material = std::make_shared<DiffuseMaterial>( 0.5, 0.5, 1.0 );

    container->add( std::make_shared<AxisAlignedSlab>( -0.5, -0.5, -0.5, 0.5, -0.5 * 1.1, +0.5 ), white_material );
    container->add( std::make_shared<AxisAlignedSlab>( -0.5, +0.5, -0.5, 0.5, +0.5 * 1.1, +0.5 ), white_material );
    container->add( std::make_shared<AxisAlignedSlab>( -0.5, -0.5, -0.5, -0.5 * 1.1, +0.5, +0.5 ), red_material );
    container->add( std::make_shared<AxisAlignedSlab>( +0.5, -0.5, -0.5, +0.5 * 1.1, +0.5, +0.5 ), blue_material );
    container->add( std::make_shared<AxisAlignedSlab>( -0.5, -0.5, -0.5, 0.5, +0.5, -0.5 * 1.1 ), white_material );
    //container->add( std::make_shared<AxisAlignedSlab>( -0.5, -0.5, +0.5, 0.5, +0.5, +0.5 * 1.1 ), white_material );

#if 0
    scene->addPointLight( PointLight( Vector4( 0.2, 0.2, 0.0 ), RGBColor( 1.0, 1.0, 1.0 ).scaled(0.15) ) );
#elif 0
    // FIXME: Something is wrong with area lights when they are close to objects.
    //        There is extra brightness for close objects, and other objects in the
    //        scene get speckles as a result.
    auto light = std::make_shared<CircleAreaLight>( 0.2, RGBColor( 1.0, 1.0, 1.0 ).scaled( 0.35 ) );
    light->transform = std::make_shared<Transform>();
    *light->transform = makeTranslation( Vector4( 0, 0.49, 0 ) );
    container->add( light );
#else
    auto box_light = std::make_shared<AxisAlignedSlab>( -0.2, 0.5, -0.2, 0.2, 0.5 * 0.95, +0.2 );
    box_light->material = std::make_shared<Material>();
    box_light->material->emittance = RGBColor( 1.0, 1.0, 1.0 );
    box_light->material->emittance.scale( 30.0 );
    container->add( box_light );
#endif

    tracer->setCameraTransform( makeTranslation( 0.0, 0.0, 1.5 ) );
    tracer->rays_per_pixel = 30;
    float focal_plane_span = 0.5;
    tracer->camera.xmin = -focal_plane_span;
    tracer->camera.xmax = +focal_plane_span;
    tracer->camera.ymin = -focal_plane_span;
    tracer->camera.ymax = +focal_plane_span;

    tracer->shader = new BasicDiffuseSpecularShader();
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(RoomSceneWithSpheres, RoomScene)
SETUP_SCENE(
    RoomScene::setup();
    tracer->rays_per_pixel = 10;
);
BUILD_SCENE(
    RoomScene::buildScene();
    auto mirror_material = std::make_shared<MirrorMaterial>();
    auto refractive_material = std::make_shared<RefractiveMaterial>( N_FLINT_GLASS );
#if 1
    container->add( std::make_shared<Sphere>( -0.3, -0.3, -0.3, 0.2 ), mirror_material );
    container->add( std::make_shared<Sphere>( +0.3, -0.3, +0.3, 0.2 ), refractive_material );
#else
    container->add( std::make_shared<Sphere>( -0.25, -0.40, 0, 0.1 ), white_material );
    container->add( std::make_shared<Sphere>( 0.0, -0.40, 0, 0.1 ), mirror_material );
    container->add( std::make_shared<Sphere>( +0.25, -0.40, 0, 0.1 ), refractive_material );
#endif
);
END_SCENE()
// ------------------------------------------------------------ 
BEGIN_SCENE( GridRoomScene )
SETUP_SCENE( TestScene::setup(); );
BUILD_SCENE(
    auto white_material = std::make_shared<DiffuseMaterial>( 1.0, 1.0, 1.0 );
    auto checkerboard_material = std::make_shared<DiffuseCheckerBoardMaterial>( 1.0, 1.0, 1.0 );

    float dim = 1.0;
    container->add( std::make_shared<AxisAlignedSlab>( -dim, 0.0, -dim, dim, -0.1, dim ), checkerboard_material ); // floor
    container->add( std::make_shared<AxisAlignedSlab>( -dim, 0.0, -dim, -dim * 1.1, dim * 2.0, dim ), white_material ); // left wall
    container->add( std::make_shared<AxisAlignedSlab>( -dim, 0.0, -dim, dim, dim * 2.0, -dim * 1.1 ), white_material ); // back wall

    auto env_map = std::make_shared<ArcLightEnvironmentMap>( Vector4(0, 1, 0), M_PI / 2.0 );
    env_map->setPower( 50.0f );
    scene->env_map = env_map;

    //container->add( std::make_shared<Sphere>( -0.0, 0.2, 0.0, 0.2 ), white_material ); // TEMP

    tracer->setCameraTransform( compose( makeRotation( M_PI / 4, Vector4(0, 1, 0) ),
                                        makeTranslation( 0.0, 0.5, 0.0 ),
                                        makeRotation( -M_PI / 8, Vector4(1, 0, 0) ),
                                        makeTranslation( 0.0, 0.0, 1.5 ))
                              );
    tracer->rays_per_pixel = 30;
    //tracer->rays_per_pixel = 100;
    float focal_plane_span = 0.5;
    tracer->camera.xmin = -focal_plane_span;
    tracer->camera.xmax = +focal_plane_span;
    tracer->camera.ymin = -focal_plane_span;
    tracer->camera.ymax = +focal_plane_span;

    tracer->shader = new BasicDiffuseSpecularShader();
);
END_SCENE()
// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(GridRoomSceneWithSpheres, GridRoomScene)
SETUP_SCENE(
    GridRoomScene::setup();
);
BUILD_SCENE(
    GridRoomScene::buildScene();
    auto white_material = std::make_shared<DiffuseMaterial>( 1.0, 1.0, 1.0 );
    auto mirror_material = std::make_shared<MirrorMaterial>();
    auto refractive_material = std::make_shared<RefractiveMaterial>( N_FLINT_GLASS );
    container->add( std::make_shared<Sphere>( -0.3, 0.2, -0.3, 0.2 ), mirror_material );
    container->add( std::make_shared<Sphere>( -0.3, 0.2, 0.3, 0.2 ), refractive_material );
    container->add( std::make_shared<Sphere>( 0.3, 0.2, -0.3, 0.2 ), white_material );
);
END_SCENE()
// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(GridRoomSceneWithBunny, GridRoomScene)
SETUP_SCENE(
    GridRoomScene::setup();
);
BUILD_SCENE(
    GridRoomScene::buildScene();
    auto white_material = std::make_shared<DiffuseMaterial>( 1.0, 1.0, 1.0 );
    auto mirror_material = std::make_shared<MirrorMaterial>();
    auto refractive_material = std::make_shared<RefractiveMaterial>( N_PLEXIGLAS );

    std::string modelPath = modelBasePath + "/stanford/bunny/reconstruction";
    auto mesh = loader.load( modelPath + "/bun_zipper_res2.ply" );
    auto bounds = mesh->getAxisAlignedBounds();
    mesh->material = mirror_material;

    mesh->transform = std::make_shared<Transform>();
    *mesh->transform = compose( makeScaling( 1.0 ),
                                makeRotation( M_PI * 0.25, Vector4( 0, 1, 0 ) ),
                                makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
    container->add( mesh );
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
    testAO1(); // White spheres
    testAO2(); // White spheres, overhead camera
    testAO3(); // White cubes
    testAO4(); // Colored cubes
    testAO5(); // Stanford Bunny
    testAO6(); // Dragon
    testAO7();
    testSphereLight1();
    testSphereLight2();
    testSphereLight3();
    testSphereLight4();
    testReflection1();
    testReflection2();
    testReflection3();
    testMesh1();         // Stanford Bunny and Dragon
    testMesh2();         // Apollo lander
    //testMeshSanMiguel();      // TODO: slow - san miguel scene
    testMeshDabrovicSponza();
    //testHairball();   // TODO: slow
    testPointLight1();
    testPointLight2();
    testPointLight3();
    testPointLight4();
    testAnimTransforms1(); // Mirror Bunny and simple shapes
    testAnimTransforms2(); // 3 Spimming Cubes and point lights
    testAnimTransforms3(); // 3 Spinning Mirror Cubes
    testAreaLight1();   // Cube with area light
    testAreaLight2();   // FIXME: assert tripping
    testCircleAreaLight1();   // Cube with circular area light
    testRefraction1();  // Mixed scene with some refractive elements
    testRefraction2();  // Mesh bunnies with varying IoR
    testRefraction3();  // Spheres of varying IoR
    testRefraction4();  // Refractive sphere with caustics
    SimpleCube::run();
    Gooch::run();
    testLogicalAND();
    testLogicalANDMesh();
    testLogicalANDLensFocusLight();
    CSGLogicalANDLensWater::run();
    CSGLogicalANDLensCrownGlass::run();
    CSGLogicalANDLensFlintGlass::run();
    RoomScene::run();
    RoomSceneWithSpheres::run();
#else
    //RoomScene::run();
    //RoomSceneWithSpheres::run();
    //GridRoomScene::run();
    //GridRoomSceneWithSpheres::run();
    GridRoomSceneWithBunny::run();
    //testLogicalAND();
    //testMesh1();         // Stanford Bunny and Dragon
    //testAnimTransforms1(); // Mirror Bunny and simple shapes
    //testRefraction2();  // Mesh bunnies with varying IoR
    //testRefraction3();  // Spheres of varying IoR
    //testAO5(); // Stanford Bunny
    //testCircleAreaLight1();   // Cube with circular area light
    //testAnimLights1();
    //testLogicalANDLensFocusLight();
    //testMeshDabrovicSponza();

#endif
    
    total_run_timer.stop();
    printf("Done - Run time = %f seconds\n", total_run_timer.elapsed());
    fflush(stdout);
    return 0;
}

