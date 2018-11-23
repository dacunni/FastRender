/*
 *  AmbientOcclusionShader.h
 *  FastRender
 *
 *  Created by David Cunningham on 2/22/14
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _AMBIENT_OCCLUSION_SHADER_H_
#define _AMBIENT_OCCLUSION_SHADER_H_

#include "Shader.h"

// Ambient occlusion shader. Could probably use lots of improvement,
// especially by acceleration structures for meshes.
class AmbientOcclusionShader : public Shader
{
    public:
        AmbientOcclusionShader() {}
        virtual ~AmbientOcclusionShader() {}

        // Shade an intersection point
        virtual void shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection );
    
};


#endif
