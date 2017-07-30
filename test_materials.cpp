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

bool animateMaterialTests = false;

// ------------------------------------------------------------ 
//                   Material Test Base
// ------------------------------------------------------------ 
BEGIN_SCENE(MaterialTestBase)
SETUP_SCENE(
    // TEMP >>> Small images for fast testing
    //image_width = image_height = 16;
    //image_width = image_height = 64;
    //image_width = image_height = 100;
    // TEMP <<<
    TestScene::setup();
    tracer->rays_per_pixel = 1;

    if( animateMaterialTests ) {
        tracer->num_frames = 32;
        tracer->loopable_animations = true;
        tracer->setCameraTransform(
                [](float anim_progress) { return compose(
                        // move up a bit
                        makeTranslation( 0.0, 0.5, 0.0 ),
                        //makeRotation( M_PI / 8.0f * sinf(2.0f*M_PI*anim_progress), Vector4(0, 1, 0) ), // wiggle
                        makeRotation( 2.0f * M_PI * anim_progress, Vector4(0, 1, 0) ), // orbit
                        // rotate so we are looking down
                        makeRotation( -0.2, Vector4(1, 0, 0) ),
                        // back away from the origin
                        makeTranslation( 0.0, 0.0, 5.0 )
                    );});
    }
    else {
        tracer->setCameraTransform( compose(
            // move up a bit
            makeTranslation( 0.0, 0.5, 0.0 ),
            // rotate so we are looking down
            makeRotation( -0.2, Vector4(1, 0, 0) ),
            // back away from the origin
            makeTranslation( 0.0, 0.0, 5.0 )
            ) );
    }
    tracer->shader = new BasicDiffuseSpecularShader();
);
std::shared_ptr<TriangleMesh> mesh;
BUILD_SCENE(
    auto floor = std::make_shared<AxisAlignedSlab>( -10, -1, -10,
                                                     10, 0, 10 );
    auto checkerboard_material = std::make_shared<DiffuseCheckerBoardMaterial>( 1.0, 1.0, 1.0 );
    container->add( floor, checkerboard_material );

    mesh = loadMaterialTestModel( loader );
    auto bounds = mesh->getAxisAlignedBounds();

    mesh->transform = std::make_shared<Transform>();
#if 1
    *mesh->transform = compose( makeScaling( 1.0 ),
                                makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ) );
#else // Mori test object
    *mesh->transform = compose( makeScaling( 1.0 ),
                                makeTranslation( Vector4( 0.0, -bounds->ymin, 0.0 ) ),
                                makeRotation( M_PI * 0.85, Vector4( 0.0, 1.0, 0.0 ) ) );
#endif
    container->add( mesh );
);
END_SCENE()

// ------------------------------------------------------------ 
//                   Ambient Occlusion
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
//                      Gooch
// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestGooch, MaterialTestBase)
SETUP_SCENE(
    MaterialTestBase::setup();
    tracer->rays_per_pixel = 5;
    tracer->shader = new GoochShader();
);
BUILD_SCENE(
    MaterialTestBase::buildScene();
);
END_SCENE()

// ------------------------------------------------------------ 
//                    Inspection
// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestInspection, MaterialTestBase)
InspectionShader * shader;
SETUP_SCENE(
    MaterialTestBase::setup();
    tracer->rays_per_pixel = 5;
    shader = new InspectionShader();
    tracer->shader = shader;
);
BUILD_SCENE(
    MaterialTestBase::buildScene();
    //mesh->material = std::make_shared<RefractiveMaterial>(N_WATER);
    mesh->material = std::make_shared<RefractiveMaterial>(N_FLINT_GLASS);
    //mesh->material = std::make_shared<RefractiveMaterial>(N_DIAMOND);
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestInspectionFresnelDialectric, MaterialTestInspection)
SETUP_SCENE(
    MaterialTestInspection::setup();
    shader->property = InspectionShader::FresnelDialectric;
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestInspectionFresnelConductor, MaterialTestInspection)
SETUP_SCENE(
    MaterialTestInspection::setup();
    shader->property = InspectionShader::FresnelConductor;
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestInspectionNormal, MaterialTestInspection)
SETUP_SCENE(
    MaterialTestInspection::setup();
    shader->property = InspectionShader::Normal;
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestInspectionIndexOfRefraction, MaterialTestInspection)
SETUP_SCENE(
    MaterialTestInspection::setup();
    shader->property = InspectionShader::IndexOfRefraction;
);
END_SCENE()

// ------------------------------------------------------------ 
//                     Point Light
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
//                     Area Light
// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestAreaLight, MaterialTestBase)
SETUP_SCENE(
    MaterialTestBase::setup();
    tracer->rays_per_pixel = 30;
);
BUILD_SCENE(
    MaterialTestBase::buildScene();
    auto light_color = RGBColor( 1.0, 1.0, 1.0 ).scaled( 1.0 );
    auto light = std::make_shared<CircleAreaLight>( 2.0, light_color );
    light->transform = std::make_shared<Transform>();
    *light->transform = compose( makeRotation( M_PI * 0.0, Vector4(0, 0, 1) ),
                                 makeTranslation( Vector4( 0, 3.0, 0 ) ) );
    container->add( light );
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestDiffuseWhiteAreaLight, MaterialTestAreaLight)
SETUP_SCENE(
    MaterialTestAreaLight::setup();
    tracer->shader = new BasicDiffuseSpecularShader();
);
BUILD_SCENE(
    MaterialTestAreaLight::buildScene();
    mesh->material = std::make_shared<DiffuseMaterial>( 1.0, 1.0, 1.0 );
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestMirrorAreaLight, MaterialTestAreaLight)
SETUP_SCENE(
    MaterialTestAreaLight::setup();
    tracer->shader = new BasicDiffuseSpecularShader();
);
BUILD_SCENE(
    MaterialTestAreaLight::buildScene();
    mesh->material = std::make_shared<MirrorMaterial>();
);
END_SCENE()

// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestRefractAreaLight, MaterialTestAreaLight)
SETUP_SCENE(
    MaterialTestAreaLight::setup();
    tracer->shader = new BasicDiffuseSpecularShader();
);
BUILD_SCENE(
    MaterialTestAreaLight::buildScene();
    mesh->material = std::make_shared<RefractiveMaterial>(N_WATER);
);
END_SCENE()

// ------------------------------------------------------------ 
//                     Arc Light
// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestArcLight, MaterialTestBase)
SETUP_SCENE(
    MaterialTestBase::setup();
    tracer->rays_per_pixel = 30;
);
BUILD_SCENE(
    MaterialTestBase::buildScene();
    scene->env_map = std::make_shared<ArcLightEnvironmentMap>();
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
//                   HDR Environment Map
// ------------------------------------------------------------ 
BEGIN_DERIVED_SCENE(MaterialTestHDREnvironmentMap, MaterialTestBase)
SETUP_SCENE(
    MaterialTestBase::setup();
    tracer->rays_per_pixel = 30;
    //tracer->rays_per_pixel = 100;
);
BUILD_SCENE(
    MaterialTestBase::buildScene();
    scene->env_map = std::make_shared<HDRImageEnvironmentMap>(env_map_filename, env_map_width, env_map_height);
);
#if 1
    std::string env_map_filename = "light_probes/debevec/stpeters_probe.float";
    unsigned int env_map_width = 1500;
    unsigned int env_map_height = 1500;
#elif 1
    std::string env_map_filename = "light_probes/debevec/rnl_probe.float";
    unsigned int env_map_width = 900;
    unsigned int env_map_height = 900;
#elif 1
    std::string env_map_filename = "light_probes/debevec/uffizi_probe.float";
    unsigned int env_map_width = 1500;
    unsigned int env_map_height = 1500;
#else
    std::string env_map_filename = "light_probes/debevec/grace_probe.float";
    unsigned int env_map_width = 1000;
    unsigned int env_map_height = 1000;
#endif
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
BEGIN_DERIVED_SCENE(MaterialTestRefractHDREnvironmentMap, MaterialTestHDREnvironmentMap)
SETUP_SCENE(
    MaterialTestHDREnvironmentMap::setup();
    tracer->shader = new BasicDiffuseSpecularShader();
);
BUILD_SCENE(
    MaterialTestHDREnvironmentMap::buildScene();
    mesh->material = std::make_shared<RefractiveMaterial>(N_WATER);
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

    if( argc > 1 ) {
        if( std::string(argv[1]) == "list" ) {
            printTests();
            exit(EXIT_SUCCESS);
        }
        else {
            // Run a specific test
            if( argc > 2 && std::string(argv[2]) == "animate" ) {
                animateMaterialTests = true;
            }
            runTest(atoi(argv[1]));
        }
    }
    else {
        // Run all tests
        printTests();
        runTests();
    }
    
    total_run_timer.stop();
    printf("Done - Run time = %f seconds\n", total_run_timer.elapsed());
    fflush(stdout);
    return 0;
}

