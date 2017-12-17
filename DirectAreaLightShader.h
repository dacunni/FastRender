#ifndef _DIRECT_AREA_LIGHT_SHADER_H_
#define _DIRECT_AREA_LIGHT_SHADER_H_

#include "Shader.h"

// Only renders direct light from area lights
class DirectAreaLightShader : public Shader
{
    public:
        DirectAreaLightShader() {}
        virtual ~DirectAreaLightShader() {}

        virtual void shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection );

};


#endif
