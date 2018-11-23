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
        BasicDiffuseSpecularShader() = default;
        virtual ~BasicDiffuseSpecularShader() = default;

        // Shade an intersection point
        virtual void shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection );

        const bool sample_area_lights = true;
        const bool sample_env_maps = true;
};


#endif
