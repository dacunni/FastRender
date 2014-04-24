/*
 *  Shader.h
 *  FastRender
 *
 *  Created by David Cunningham on 2/22/14
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SHADER_H_
#define _SHADER_H_

class RandomNumberGenerator;
class RayIntersection;
class Scene;

class Shader 
{
    public:
        Shader() {}
        virtual ~Shader() {}

        // Shade an intersection point
        virtual void shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection ) = 0;
    
};


#endif