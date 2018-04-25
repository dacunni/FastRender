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

    void renderEditCameraPerspective();

    // Callbacks
    void viewportReshaped( int w, int h );
    void repaintViewport();
    void keyPressed( unsigned char key, int x, int y );
    void keyReleased( unsigned char key, int x, int y );
    void mouseButton( int button, int state, int x, int y );
    void mouseMotionWhileButtonPressed( int x, int y );
    void animTimer( int value );
    void userTimerUpdate( double timeNow, double deltaTime );

    std::shared_ptr<Scene> scene;
    EditorSceneGraph editorScene;

protected:
    unsigned int windowWidth = 512;
    unsigned int windowHeight = 512;

private:
    static void sViewportReshaped( int w, int h );
    static void sRepaintViewport();
    static void sKeyPressed( unsigned char key, int x, int y );
    static void sKeyReleased( unsigned char key, int x, int y );
    static void sMouseButton( int button, int state, int x, int y );
    static void sMouseMotionWhileButtonPressed( int x, int y );
    static void sAnimTimer( int value );

    Transform cameraTranslation();
    Transform cameraRotation();
    Transform cameraTransform();
    Vector4 cameraForward();
    Vector4 cameraRight();
    Vector4 cameraUp();
    void updateEditCamera();

    ShaderProgram defaultShaderProgram;
    ShaderProgram wireMeshEdgeShaderProgram;

    float update_rate_sec = 1.0f / 20.0f; // 20 FPS
    WallClockTimer runTimer;
    double animTime = 0.0;

    // Editor camera
    struct EditCameraParams {
        // Transform
        Vector4 position = Vector4( 0.0, 0.0, 25.0 );
        float xRotation = 0.0;
        float yRotation = 0.0;
        // Control speed
        float translationSpeed = 5.0;
        float keyboardRotationSpeed = 0.25;
        float mouseRotationSpeed = 0.001;
        // Camera Parameters
        float xmin = -0.15;
        float xmax = +0.15;
        float ymin = -0.15;
        float ymax = +0.15;
    } editCameraParams;

    SimpleCamera editCamera;

    int mouseButtonState[3] = { 
        GLUT_UP, // left
        GLUT_UP, // right
        GLUT_UP  // middle
    };
    unsigned char keyState[256] = {0};
    int mouseLastX = -1;
    int mouseLastY = -1;

    RandomNumberGenerator rng;

    bool drawWireframes = false;
};

#endif

