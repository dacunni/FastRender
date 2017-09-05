#ifndef _OPENGL_UTIL_H_
#define _OPENGL_UTIL_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>     // Core OpenGL 3.x+
#include <GLUT/glut.h>      // GLUT + OpenGL
#else
#include <GL/glut.h>
#include <GL/gl.h>
#endif

#define GL_WARN_IF_ERROR() warnIfError( __FUNCTION__, __LINE__ )

GLenum warnIfError( const char * func, int line ) 
{
    GLenum err = glGetError();

    if( err != GL_NO_ERROR ) {
        printf("GL Error: %s:%d %s\n", func, line, gluErrorString(err));
    }

    return err;
}


#endif
