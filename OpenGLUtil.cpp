/*
 *  OpenGLUtil.pp
 *  FastRender
 *
 *  Created by David Cunningham on 5/12/2014
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>
#include "OpenGLUtil.h"

GLenum warnIfError( const char * func, int line ) 
{
    GLenum err = glGetError();

    if( err != GL_NO_ERROR ) {
        printf("GL Error: %s:%d %s\n", func, line, gluErrorString(err));
    }

    return err;
}


