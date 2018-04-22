/*
 *  GoochShader.h
 *  FastRender
 *
 *  Created by David Cunningham on 5/17/16
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _GOOCH_SHADER_H_
#define _GOOCH_SHADER_H_

#include "Shader.h"

class GoochShader : public Shader
{
    public:
        GoochShader() {}
        virtual ~GoochShader() {}

        // Shade an intersection point
        virtual void shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection );
    
};


#endif
