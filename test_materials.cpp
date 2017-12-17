#include <iostream>
#include <sys/stat.h>
#include <stdio.h>

#include "FastRender.h"

std::string output_path = "testoutput";

#include "test_scenes/materials/MaterialTestBase.scene"
#include "test_scenes/materials/MaterialTestMontageBase.scene"
#include "test_scenes/materials/MaterialTestMontageTwoParamBase.scene"
#include "test_scenes/materials/MaterialTestAmbientOcclusion.scene"
#include "test_scenes/materials/MaterialTestGooch.scene"
#include "test_scenes/materials/MaterialTestInspection.scene"
#include "test_scenes/materials/MaterialTestPointLight.scene"
#include "test_scenes/materials/MaterialTestAreaLight.scene"
#include "test_scenes/materials/MaterialTestArcLight.scene"
#include "test_scenes/materials/MaterialTestHDREnvironmentMap.scene"
#include "test_scenes/materials/MaterialTestCookTorrancePointLight.scene"
#include "test_scenes/materials/MaterialTestCookTorranceAreaLight.scene"

int main (int argc, char * const argv[]) 
{
    printf("Material Tests\n");
    fflush(stdout);

    mkdir(output_path.c_str(), 0777);

    Timer total_run_timer;
    total_run_timer.start();

    //RandomNumberGenerator rng;
    //rng.seedCurrentTime();

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

