#ifndef _INSPECTION_SHADER_H_
#define _INSPECTION_SHADER_H_

#include "Shader.h"

// Utility shader for debugging lighting by allowing the visualization of
// various surface and lighting properties of scene objects.
class InspectionShader : public Shader
{
    public:
        InspectionShader() = default;
        virtual ~InspectionShader() = default;

        virtual void shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection );

        enum Property {
            FresnelDialectric = 0,
            FresnelConductor,
            Normal,
            IndexOfRefraction,
            TextureUVCoordinate,
            NumProperties
        };

        Property property = FresnelDialectric;
};


#endif
