#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <memory>
#include "OpenGLUtil.h"
#include "EditorSceneGraph.h"
#include "ShaderProgram.h"
#include "Timer.h"
#include "SimpleCamera.h"
#include "RandomNumberGenerator.h"

class Editor {
public:
    Editor();
    virtual ~Editor();
    
    void init();
    void buildGpuBuffers();
    void start();

    void viewportReshaped( int w, int h );
    void repaintViewport();
    void keyPressed( unsigned char key, int x, int y );
    void mouseButton( int button, int state, int x, int y );
    void mouseMotionWhileButtonPressed( int x, int y );
    void animTimer( int value );

    std::shared_ptr<Scene> scene;
    EditorSceneGraph editorScene;

protected:
    unsigned int windowWidth = 600;
    unsigned int windowHeight = 400;

private:
    static void sViewportReshaped( int w, int h );
    static void sRepaintViewport();
    static void sKeyPressed( unsigned char key, int x, int y );
    static void sMouseButton( int button, int state, int x, int y );
    static void sMouseMotionWhileButtonPressed( int x, int y );
    static void sAnimTimer( int value );

    void updateEditCamera();

    ShaderProgram defaultShaderProgram;

    float update_rate_sec = 1.0f / 20.0f; // 20 FPS
    WallClockTimer runTimer;

    // Editor camera
    struct EditCameraParams {
        float az = M_PI / 4.0f;
        float el = -0.2;
        float azStep = 0.1;
        float elStep = 0.1;
        Vector4 position = Vector4(2.0, 2.0, 25.0);
        Vector4 positionStep = Vector4(1.0, 1.0, 1.0);
        float xmin = -0.15;
        float xmax = +0.15;
        float ymin = -0.15;
        float ymax = +0.15;
    } editCameraParams;
    SimpleCamera editCamera;
    RandomNumberGenerator rng;

    bool drawWireframes = false;
};

#endif

