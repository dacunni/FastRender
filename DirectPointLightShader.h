#ifndef _DIRECT_POINT_LIGHT_SHADER_H_
#define _DIRECT_POINT_LIGHT_SHADER_H_

#include "Shader.h"

// Only renders direct light from point lights
class DirectPointLightShader : public Shader
{
    public:
        DirectPointLightShader() {}
        virtual ~DirectPointLightShader() {}

        virtual void shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection );

};


#endif
