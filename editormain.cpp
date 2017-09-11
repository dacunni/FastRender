#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

#include "FastRender.h"

int window_width = 350;
int window_height = 350;

void viewportReshaped( int width, int height ) 
{
    //printf("reshape: %d x %d\n", width, height);
    window_width = width;
    window_height = height;
    glViewport( 0, 0, window_width, window_height );
    GL_WARN_IF_ERROR();
}

void repaintViewport( void ) 
{
    //printf("repaint\n");
    glClearColor( 0.2, 0.2, 0.3, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );

    // TODO

    glDisable( GL_DEPTH_TEST );
    glutSwapBuffers();
}

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

int main (int argc, char * const argv[]) 
{
    Editor editor;

    editor.init();

    editor.scene = buildScene();
    editor.editorScene.build(*editor.scene);
    editor.editorScene.print();

    editor.buildGpuBuffers();

    editor.start();

    return EXIT_SUCCESS;
}

