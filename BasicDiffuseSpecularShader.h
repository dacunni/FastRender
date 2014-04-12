/*
 *  BasicDiffuseSpecularShader.h
 *  FastRender
 *
 *  Created by David Cunningham on 4/12/14
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _BASIC_DIFFUSE_SPECULAR_SHADER_H_
#define _BASIC_DIFFUSE_SPECULAR_SHADER_H_

#include "Shader.h"

// Super basic traditional diffuse + specular shader
class BasicDiffuseSpecularShader : public Shader
{
    public:
        BasicDiffuseSpecularShader() {}
        virtual ~BasicDiffuseSpecularShader() {}

        // Shade an intersection point
        virtual void shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection );
    
};


#endif
