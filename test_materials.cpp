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
    floor->transform = std::make_shared<Transform>();
    *floor->transform = makeTranslation( Vector4( -1.0, 0.0, 0.0 ) );
    container->add( floor );

    mesh = loadMaterialTestModel( loader );
    if( !mesh ) { return; }
    AxisAlignedSlab * bounds = mesh->getAxisAlignedBounds();

    TMOctreeAccelerator * mesh_octree = new TMOctreeAccelerator( *dynamic_cast<TriangleMesh*>(mesh) );
    mesh_octree->build();
    mesh->accelerator = mesh_octree;
    mesh->transform = std::make_shared<Transform>();
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
    mesh->material = std::make_shared<DiffuseMaterial>( 1.0, 1.0, 1.0 );
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
    mesh->material = std::make_shared<MirrorMaterial>();
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
    mesh->material = std::make_shared<RefractiveMaterial>(N_WATER);
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
    mesh->material = std::make_shared<DiffuseMaterial>( 1.0, 1.0, 1.0 );
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
    mesh->material = std::make_shared<MirrorMaterial>();
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
    mesh->material = std::make_shared<RefractiveMaterial>(N_WATER);
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
    mesh->material = std::make_shared<RefractiveMaterial>(N_DIAMOND);
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
    mesh->material = std::make_shared<DiffuseMaterial>( 1.0, 1.0, 1.0 );
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
    mesh->material = std::make_shared<MirrorMaterial>();
);
END_SCENE()


// ------------------------------------------------------------ 
// Test runner
// ------------------------------------------------------------ 
int main (int argc, char * const argv[]) 
{
    printf("Material Tests\n");
    fflush(stdout);

    mkdir(output_path.c_str(), 0777);

    Timer total_run_timer;
    total_run_timer.start();

    rng.seedCurrentTime();
    
    // Tests
#if 1
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
#endif
    
    total_run_timer.stop();
    printf("Done - Run time = %f seconds\n", total_run_timer.elapsed());
    fflush(stdout);
    return 0;
}

