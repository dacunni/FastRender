#ifndef _DIRECT_ENVIRONMENT_MAP_SHADER_H_
#define _DIRECT_ENVIRONMENT_MAP_SHADER_H_

#include "Shader.h"

// Only renders direct light from an environment map
class DirectEnvironmentMapShader : public Shader
{
    public:
        DirectEnvironmentMapShader() {}
        virtual ~DirectEnvironmentMapShader() {}

        virtual void shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection );

};


#endif
