#ifndef _COOK_TORRANCE_MATERIAL_H_
#define _COOK_TORRANCE_MATERIAL_H_

#include "Material.h"

class CookTorranceMaterial : public Material
{
    public:
        CookTorranceMaterial();
        CookTorranceMaterial( float r, float g, float b );
        CookTorranceMaterial( float r, float g, float b, float roughness );
        virtual ~CookTorranceMaterial() {}

        virtual float specularity() { return F0(); }

        float F0() const { return fresnel_F0; }
        void setFresnelAtNormal(float f0) { fresnel_F0 = f0; }

        virtual float BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo ) const;
        virtual DistributionSample sampleBxDF( RandomNumberGenerator & rng,
                                               const RayIntersection & intersection ) const;

        float roughness; // [0, 1]
        float fresnel_F0 = 0.3;
};


#endif
