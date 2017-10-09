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

class PointLight;
class AreaLight;
class RGBColor;

// Super basic traditional diffuse + specular shader
class BasicDiffuseSpecularShader : public Shader
{
    public:
        BasicDiffuseSpecularShader() {}
        virtual ~BasicDiffuseSpecularShader() {}

        // Shade an intersection point
        virtual void shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection );

        inline RGBColor samplePointLights( const Scene & scene,
                                           const RayIntersection & intersection );
        inline RGBColor samplePointLight( const Scene & scene,
                                          const RayIntersection & intersection,
                                          const PointLight & light );
    
};


#endif
