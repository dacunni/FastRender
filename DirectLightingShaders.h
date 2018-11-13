#ifndef _DIRECT_LIGHTING_SHADERS_H_
#define _DIRECT_LIGHTING_SHADERS_H_

#include "Shader.h"

// Only renders direct light from point lights
class DirectPointLightShader : public Shader
{
    public:
        DirectPointLightShader() = default;
        virtual ~DirectPointLightShader() = default;

        virtual void shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection );

};

// Only renders direct light from area lights
class DirectAreaLightShader : public Shader
{
    public:
        DirectAreaLightShader() = default;
        virtual ~DirectAreaLightShader() = default;

        virtual void shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection );

};

// Only renders direct light from an environment map
class DirectEnvironmentMapShader : public Shader
{
    public:
        DirectEnvironmentMapShader() = default;
        virtual ~DirectEnvironmentMapShader() = default;

        virtual void shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection );

};

#endif
