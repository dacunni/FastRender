//
//  PreviewWindow.h
//  FastRender
//
//  Created by David Cunningham on 6/24/17.
//
//

#ifndef __FastRender__PreviewWindow__
#define __FastRender__PreviewWindow__

#include <memory>
#include <vector>

#include "OpenGLUtil.h"
#include "ShaderProgram.h"

class Artifacts;

class PreviewWindow {
public:
    PreviewWindow( Artifacts & a );
    virtual ~PreviewWindow();
    
    void init();

    void viewportReshaped( int w, int h );
    void repaintViewport();
    void keyPressed( unsigned char key, int x, int y );
    void mouseButton( int button, int state, int x, int y );
    void mouseMotionWhileButtonPressed( int x, int y );
    void animTimer( int value );

protected:
    int window_width;
    int window_height;

    int button_state[5];

private:
    static void sViewportReshaped( int w, int h );
    static void sRepaintViewport();
    static void sKeyPressed( unsigned char key, int x, int y );
    static void sMouseButton( int button, int state, int x, int y );
    static void sMouseMotionWhileButtonPressed( int x, int y );
    static void sAnimTimer( int value );

    void printValuesAt( int win_x, int win_y );

    enum ImageArtifact {
        FramebufferImage = 0, NormalsImage, DepthImage,
        NumImageArtifacts
    };
    ImageArtifact activeImage = FramebufferImage;
    Artifacts & artifacts;

    ShaderProgram imgShaderProgram;

    GLuint img_vao = 0;
    GLuint img_vbo = 0;
    GLuint pixelAccumTex = 0;
    GLuint pixelCountTex = 0;

    //float update_rate_sec = 0.3;
    float update_rate_sec = 0.03;
    
    // Image adjustments
    float gain = 1.0f;
    float gainAdjustMultiplier = 1.15;
    float bias = 0.0f;
    float biasAdjustIncrement = 0.05;
    bool flagUnnaturalValues = false;
};

#endif /* defined(__FastRender__PreviewWindow__) */
