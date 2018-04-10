#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

#include "FastRender.h"

// TEMP - A test scene for developing the UI
std::shared_ptr<Scene> buildScene()
{
    auto scene = std::make_shared<Scene>();

    Timer build_scene_timer;
    build_scene_timer.start();
	auto container = std::make_shared<FlatContainer>();
	
    addOffsetCubes( container );
    addBunny( container );
    addGroundPlane( container );

    auto sphere = std::make_shared<Sphere>( Vector4( -3.0, 1.0, -7.0 ), 1.0 );
    sphere->material = std::make_shared<MirrorMaterial>();
    container->add( sphere );

    auto sphere2 = std::make_shared<Sphere>( Vector4( -1.0, 0.1, -0.0 ), 0.25 );
    sphere2->material = std::make_shared<DiffuseMaterial>( 1.0, 0.0, 1.0 );
    container->add( sphere2 );

    auto cubeContainer = std::make_shared<FlatContainer>();
    // Cubes for reference
    auto cube1 = std::make_shared<AxisAlignedSlab>( 1.0, -0.5, -3.0, 0.5 );
    cube1->material = std::make_shared<DiffuseMaterial>( 0.0, 0.0, 1.0 );
    cube1->transform = std::make_shared<Transform>();
    *cube1->transform = makeTranslation( Vector4( 0.1, 0.5, -0.5 ) );
    cubeContainer->add( cube1 );
    auto cube2 = std::make_shared<AxisAlignedSlab>( 1.0, -0.5+1.0, -3.0, 0.5 );
    cube2->material = std::make_shared<DiffuseMaterial>( 0.0, 1.0, 0.0 );
    cubeContainer->add( cube2 );
    container->add( cubeContainer );

	scene->root = container;
    scene->env_map = std::make_shared<ArcLightEnvironmentMap>();
    scene->buildLightList();
    build_scene_timer.stop();
    printf( "Build scene: %f seconds\n", build_scene_timer.elapsed() );

    return scene;
}

// TEMP
std::string output_path = "testoutput";
const std::string modelBasePath = "models";
#include "test_scenes/render/SanMiguel.scene"
#include "test_scenes/render/DabrovicSponza.scene"
#include "test_scenes/render/SimpleCube.scene"
#include "test_scenes/render/BunnyEtcDiffusePointLights.scene"
#include "test_scenes/render/RefractiveSpheresAndCubes.scene"
#include "test_scenes/render/RefractiveSpheresVaryingIOR.scene"
#include "test_scenes/render/GridRoom.scene"
#include "test_scenes/render/MirrorSphereColoredSpheresArcLight.scene"
#include "test_scenes/render/UVMesh.scene"

int main (int argc, char * const argv[]) 
{
    Editor editor;

    editor.init();

#if 1
    //RefractiveSpheresVaryingIOR test(output_path);
    DabrovicSponza test(output_path);
    //SanMiguel test(output_path);
    //GridRoomSceneWithBunny test(output_path);
    //MirrorSphereColoredSpheresArcLight test(output_path);
    //UVMesh test(output_path);
    test.setup();
    test.buildScene();
    editor.scene = std::shared_ptr<Scene>(test.scene);
#else
    editor.scene = buildScene();
#endif
    editor.editorScene.build(*editor.scene);
    editor.editorScene.print();

    editor.buildGpuBuffers();

    editor.start();

    return EXIT_SUCCESS;
}

