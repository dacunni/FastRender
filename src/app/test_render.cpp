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

#include "test_scenes/render/AmbientOcclusion.scene"
#include "test_scenes/render/Logical.scene"
#include "test_scenes/render/CSGLogicalANDLens.scene"
#include "test_scenes/render/DabrovicSponza.scene"
#include "test_scenes/render/Hairball.scene"
#include "test_scenes/render/Gooch.scene"
#include "test_scenes/render/Room.scene"
#include "test_scenes/render/GridRoom.scene"
#include "test_scenes/render/RefractProfile.scene"
#include "test_scenes/render/BunnyEtcVariousMaterialsPointLights.scene"
#include "test_scenes/render/EmissiveSphereLight.scene"
#include "test_scenes/render/RefractiveSpheresAndCubes.scene"
#include "test_scenes/render/RefractiveBunniesVaryingIOR.scene"
#include "test_scenes/render/RefractiveSpheresVaryingIOR.scene"
#include "test_scenes/render/RefractiveSphereEmissiveObjectCaustic.scene"


void testMesh1()
{
    int imageSize = 64;
    //int imageSize = 256;
    //int imageSize = 320;
    //int imageSize = 512;
    //int imageSize = 1024;
    int imageWidth = imageSize * 2, imageHeight = imageSize;
    ImageTracer tracer( imageWidth, imageHeight, 1, 25 );
    tracer.camera->setFocalPlaneDimensions( 0.6, 0.3 );
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
    //scene->env_map = std::make_shared<HDRImageEnvironmentMap>("light_probes/debevec/stpeters_probe.float",
    //                                                          1500,
    //                                                          1500);
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
    env_map->setPower( 1.0f );
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
    auto light_color = RGBColor( 1.0, 1.0, 1.0 ).scaled( 1.0 );
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

// Test with a real circular area light
void testCircleAreaLight2()
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
    //container->add( floor );

    auto cube = std::make_shared<AxisAlignedSlab>( -0.5, -0.5, -0.5, 1.0 );
    cube->transform = std::make_shared<Transform>();
    *cube->transform = compose( makeTranslation( Vector4( 0.0, 0.5, 0.0 ) ),
                                makeRotation( -0.8 * M_PI, Vector4(0, 1, 0) ) );
    //container->add( cube );

    float sradius = 0.5;
    auto sphere = std::make_shared<Sphere>( 0.0, 0.0, 0.0, sradius );
    container->add( sphere );

    std::string modelPath = modelBasePath + "/stanford/bunny/reconstruction";
    auto mesh = loader.load( modelPath + "/bun_zipper_res2.ply" );
    auto bounds = mesh->getAxisAlignedBounds();

    mesh->transform = std::make_shared<Transform>();
    *mesh->transform = compose( makeScaling( 1.5, 1.5, 1.5 ),
                                makeTranslation( Vector4( 0.0, 0.0, 0.0 ) ) );
    //container->add( mesh );

    // Area Light
    float light_radius = 1.0;
    float light_distance = 0.7;
    auto light = std::make_shared<CircleAreaLight>( light_radius, RGBColor( 1.0, 0.4, 0.2 ).scaled( 15.0 ) );
    light->transform = std::make_shared<Transform>();
    *light->transform = compose( makeRotation( M_PI * -0.0, Vector4(0, 0, 1) ),
                                 makeTranslation( Vector4( 0, light_distance, 0 ) ) );
    container->add( light );
    // Area Light
    light = std::make_shared<CircleAreaLight>( light_radius, RGBColor( 1, 1, 1 ).scaled( 15.0 ) );
    light->transform = std::make_shared<Transform>();
    *light->transform = compose( makeRotation( M_PI * -1.0, Vector4(0, 0, 1) ),
                                 makeTranslation( Vector4( 0, light_distance, 0 ) ) );
    container->add( light );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_circle_area_light2_";

    // Camera back and rotated a bit around x so we're looking slightly down
    //Transform rotation = makeRotation( -M_PI / 8, Vector4(1, 0, 0) );
    Transform rotation = makeRotation( 0.0, Vector4(1, 0, 0) );
    Transform translation = makeTranslation( 0.0, 0.0, 8.0 );
    tracer.setCameraTransform( compose( rotation, translation ) );

    tracer.scene->buildLightList();
    tracer.render();
}

// Test with a real rectangle area light
void testRectangleAreaLight1()
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
    auto light_color = RGBColor( 1.0, 1.0, 1.0 ).scaled( 1.0 );
    auto light = std::make_shared<RectangleAreaLight>( 2.0, 2.0, light_color );
    light->transform = std::make_shared<Transform>();
    *light->transform = compose( makeRotation( M_PI * 0.0, Vector4(0, 0, 1) ),
                                 makeTranslation( Vector4( 0, 2.5, 0 ) ) );
    container->add( light );

	scene->root = container;
    tracer.scene = scene;

    tracer.shader = new BasicDiffuseSpecularShader();

    tracer.artifacts.output_path = output_path;
    tracer.artifacts.file_prefix = "test_rectangle_area_light1_";

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
    auto light_color = RGBColor( 1.0, 1.0, 1.0 ).scaled( 1.0 );
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
// ------------------------------------------------------------ 
// Test runner
// ------------------------------------------------------------ 
int main (int argc, char * const argv[]) 
{
    printf("Render Tests\n");
    fflush(stdout);

    Config config;
    if(!loadConfigFromKeyValueFile("render.config", config)) {
        return EXIT_FAILURE;
    }

    auto logger = std::make_shared<FileLogger>("render.log");
    logger->mirrorToStdout = config.get<bool>("MIRROR_LOGGING_TO_STDOUT");
    setLogger(logger);

    config.log(*logger);

    mkdir(output_path.c_str(), 0777);

    // Tests
#if 0
    testMesh1();         // Stanford Bunny and Dragon
    testMesh2();         // Apollo lander
    //testMeshSanMiguel();      // TODO: slow - san miguel scene
    testMeshDabrovicSponza();
    //testHairball();   // TODO: slow
    testAnimTransforms1(); // Mirror Bunny and simple shapes
    testAnimTransforms2(); // 3 Spimming Cubes and point lights
    testAnimTransforms3(); // 3 Spinning Mirror Cubes
    testAreaLight1();   // Cube with area light
    testAreaLight2();   // FIXME: assert tripping
    testCircleAreaLight1();   // Cube with circular area light
    testCircleAreaLight2();   // Area light proximity test
    testRectangleAreaLight1();   // Cube with rectangle area light
#elif 0
    // NEW scene file format
    TestScene testScene("testoutput", "testname");
    std::string sceneRoot("test_scenes/render/");
    std::string ext(".scn");
    //std::string testName("UVMesh");
    //std::string testName("TexturedMesh");
    //std::string testName("SimpleCube");
    //std::string testName("SpheresPointLight");
    //std::string testName("SpheresColoredPointLights");
    //std::string testName("SanMiguel");
    //std::string testName("MirrorSphereColoredSpheresArcLight");
    std::string testName("BunnyEtcDiffusePointLights");

    //bool ok = loadTestSceneFromFile("test_scenes/render/SpheresColoredPointLights.scn", testScene);
    bool ok = loadTestSceneFromFile(sceneRoot + testName + ext, testScene);
    std::cout << "loadTestSceneFromFile -> " << ok << std::endl;
    if(!ok) { exit(EXIT_FAILURE); }
    testScene.render();

#else
    if( argc > 1 ) {
        if( std::string(argv[1]) == "list" ) {
            printTests();
            exit(EXIT_SUCCESS);
        }
        else {
            // Run a specific test

#if 0       // TODO - replicate these features in render tests
            if( argc > 2 && std::string(argv[2]) == "animate" ) {
                animateMaterialTests = true;
            }
            if( argc > 2 && std::string(argv[2]) == "stereo" ) {
                stereoMaterialTests = true;
            }
            if( argc > 2 && std::string(argv[2]) == "preview" ) {
                showPreviewWindow = true;
            }
#endif
            int first, last;
            if(sscanf(argv[1], "%d-%d", &first, &last) == 2) {
                for(int index = first; index <= last; index++) {
                    runTest(index);
                }
            }
            else {
                runTest(atoi(argv[1]));
            }
        }
    }
    else {
        // Run all tests
        printTests();
        logTests(*logger);
        runTests();
    }

#endif
    
    fflush(stdout);
    return 0;
}

