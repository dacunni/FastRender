/*
 *  OpenGLUtil.h
 *  FastRender
 *
 *  Created by David Cunningham on 4/11/2014
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _OPENGL_UTIL_H_
#define _OPENGL_UTIL_H_

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
