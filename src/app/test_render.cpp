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
#include "test_scenes/render/GridRoom.scene"
#include "test_scenes/render/RefractProfile.scene"
#include "test_scenes/render/EmissiveSphereLight.scene"
#include "test_scenes/render/RefractiveSphereEmissiveObjectCaustic.scene"

// TODO: Implement time varying transforms before removing these scenes
#include "test_scenes/render/Room.scene"
#include "test_scenes/render/RefractiveSpheresAndCubes.scene"

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
#if 1
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
    //std::string testName("BunnyEtcDiffusePointLights");
    //std::string testName("BunnyEtcVariousMaterialsPointLights");
    //std::string testName("RefractiveSpheresVaryingIOR");
    //std::string testName("RefractiveBunniesVaryingIOR");
    //std::string testName("Room");
    std::string testName("RoomWithSpheres");
    //std::string testName("RoomWithSpheresCookTorrance");
    //std::string testName("Gooch");
    //std::string testName("Hairball");

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

