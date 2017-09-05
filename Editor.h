#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <memory>
#include "OpenGLUtil.h"
#include "EditorSceneGraph.h"
#include "ShaderProgram.h"

class Editor {
public:
    Editor();
    virtual ~Editor();
    
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

    ShaderProgram defaultShaderProgram;

    // TODO - remnants of PreviewWindow heritage. change appropriately
    GLuint img_shader_program = 0;
    GLuint img_vertex_shader = 0;
    GLuint img_fragment_shader = 0;
    GLuint img_vao = 0;
    GLuint img_vbo = 0;
    GLuint pixelAccumTex = 0;
    GLuint pixelCountTex = 0;

    float update_rate_sec = 0.3;
};

#endif

