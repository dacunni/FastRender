#include <iostream>
#include <sys/stat.h>
#include <stdio.h>

#include "FastRender.h"

std::string output_path = "testoutput";

#include "test_scenes/materials/MaterialTestBase.scene"
#include "test_scenes/materials/MaterialTestMontageBase.scene"
#include "test_scenes/materials/MaterialTestMontageTwoParamBase.scene"
#include "test_scenes/materials/MaterialTestNullShader.scene"
#include "test_scenes/materials/MaterialTestAmbientOcclusion.scene"
#include "test_scenes/materials/MaterialTestGooch.scene"
#include "test_scenes/materials/MaterialTestInspection.scene"
#include "test_scenes/materials/MaterialTestPointLight.scene"
#include "test_scenes/materials/MaterialTestAreaLight.scene"
#include "test_scenes/materials/MaterialTestArcLight.scene"
#include "test_scenes/materials/MaterialTestHDREnvironmentMap.scene"
#include "test_scenes/materials/MaterialTestHotBoxEnvironmentMap.scene"
#include "test_scenes/materials/MaterialTestCookTorrancePointLight.scene"
#include "test_scenes/materials/MaterialTestCookTorranceAreaLight.scene"
#include "test_scenes/materials/MaterialTestCookTorranceEnvironmentMap.scene"

int main (int argc, char * const argv[]) 
{
    printf("Material Tests\n");
    fflush(stdout);

    Config config;
    loadConfigFromKeyValueFile("render.config", config);

    auto logger = std::make_shared<FileLogger>("render.log");
    logger->mirrorToStdout = config.get<bool>("MIRROR_LOGGING_TO_STDOUT");
    setLogger(logger);

    config.log(*logger);
    mkdir(output_path.c_str(), 0777);

    Timer total_run_timer;
    total_run_timer.start();

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
            if( argc > 2 && std::string(argv[2]) == "stereo" ) {
                stereoMaterialTests = true;
            }
            if( argc > 2 && std::string(argv[2]) == "preview" ) {
                showPreviewWindow = true;
            }
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
    
    total_run_timer.stop();
    printf("Done - Run time = %f seconds\n", total_run_timer.elapsed());
    fflush(stdout);
    return 0;
}

