#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

#include "FastRender.h"

// TEMP
std::string output_path = "testoutput";
const std::string modelBasePath = "models";
#include "test_scenes/render/SanMiguel.scene"
#include "test_scenes/render/DabrovicSponza.scene"
#include "test_scenes/render/SimpleCube.scene"
#include "test_scenes/render/BunnyEtcDiffusePointLights.scene"
#include "test_scenes/render/RefractiveSpheresAndCubes.scene"
#include "test_scenes/render/RefractiveSpheresVaryingIOR.scene"
#include "test_scenes/render/Room.scene"
#include "test_scenes/render/GridRoom.scene"
#include "test_scenes/render/MirrorSphereColoredSpheresArcLight.scene"
#include "test_scenes/render/UVMesh.scene"
#include "test_scenes/render/Hairball.scene"

#include "test_scenes/materials/MaterialTestBase.scene"
#include "test_scenes/materials/MaterialTestMontageBase.scene"
#include "test_scenes/materials/MaterialTestMontageTwoParamBase.scene"
#include "test_scenes/materials/MaterialTestCookTorranceAreaLight.scene"
#include "test_scenes/materials/MaterialTestCookTorranceEnvironmentMap.scene"

int main (int argc, char * const argv[]) 
{
    Editor editor;

    editor.init("Editor");

    //BunnyEtcDiffusePointLights test(output_path);
    //RefractiveSpheresVaryingIOR test(output_path);
    //DabrovicSponza test(output_path);
    //SanMiguel test(output_path);
    //GridRoomSceneWithBunny test(output_path);
    //MaterialTestCookTorranceRoughness01WhiteAreaLight test(output_path);
    //MaterialTestCookTorranceRoughness05WhiteAreaLight test(output_path);
    //MaterialTestCookTorranceRoughness10WhiteAreaLight test(output_path);
    //MaterialTestCookTorranceRoughness05WhiteEnvironmentMap test(output_path);
    //MaterialTestCookTorranceAreaLightSweepRoughnessLowVaryingColors test(output_path);
    //GridRoomSceneWithLunarLander test(output_path);
    //GridRoomSceneWithTieFighter test(output_path);
    //MirrorSphereColoredSpheresArcLight test(output_path);
    //UVMesh test(output_path);
    //TexturedMesh test(output_path);
    //Hairball test(output_path);
    RoomSceneWithSpheres test(output_path);

    test.setup();
    test.buildScene();
    editor.scene = std::shared_ptr<Scene>(test.scene);
    editor.editorScene.build(*editor.scene);
    //editor.editorScene.print();
    editor.buildGpuBuffers();
    editor.start();

    return EXIT_SUCCESS;
}

