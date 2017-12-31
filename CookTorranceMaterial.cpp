
#include <typeinfo>
#include "CookTorranceMaterial.h"
#include "RandomNumberGenerator.h"
#include "GeometryUtils.h"

static float DEFAULT_ROUGHNESS = 0.1;
static float DEFAULT_SPECULAR = 0.2;

CookTorranceMaterial::CookTorranceMaterial()
  : Material(),
    roughness(DEFAULT_ROUGHNESS),
    specular(DEFAULT_SPECULAR)
{
    diffuseColor.setRGB( 1.0f, 1.0f, 1.0f );
    specularColor.setRGB( 0.0f, 0.0f, 0.0f ); 
    emittance.setRGB( 0.0f, 0.0f, 0.0f );
}

CookTorranceMaterial::CookTorranceMaterial( float r, float g, float b )
  : Material(),
    roughness(DEFAULT_ROUGHNESS),
    specular(DEFAULT_SPECULAR)
{ 
    diffuseColor.setRGB( r, g, b ); 
    specularColor.setRGB( 0.0f, 0.0f, 0.0f ); 
    emittance.setRGB( 0.0f, 0.0f, 0.0f );
}

CookTorranceMaterial::CookTorranceMaterial( float r, float g, float b, float _roughness )
  : Material(),
    roughness(_roughness),
    specular(DEFAULT_SPECULAR)
{ 
    diffuseColor.setRGB( r, g, b ); 
    specularColor.setRGB( 0.0f, 0.0f, 0.0f ); 
    emittance.setRGB( 0.0f, 0.0f, 0.0f );
}

DistributionSample
CookTorranceMaterial::sampleBxDF( RandomNumberGenerator & rng,
                                  const RayIntersection & intersection ) const
{
    DistributionSample sample;
#if 0
    if(roughness < 0.01) {
        // TODO: Do we need to handle 0 roughness like a mirror?
        // Perfect mirror - PDF if a Dirac delta function
        sample.direction = mirror( intersection.fromDirection(), intersection.normal );
        sample.direction.makeDirection();
        sample.pdf_sample = DistributionSample::DIRAC_PDF_SAMPLE;
    }
    else {
#endif
        // TODO: Sample the distribution. This is sampling like a perfectly diffuse distribution
#ifdef USE_COSINE_SAMPLING
        return sampleCosineLobe(rng, intersection);
#else
        return sampleHemisphereUniform(rng, intersection);
#endif
#if 0
    }
#endif
    return sample;
}

float
CookTorranceMaterial::BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo ) const
{
    const float NdV = dot(normal, wi);
    const float NdL = dot(normal, wo);

    // Make sure wi/wo are on the same side as the normal
    if(NdV < 0.0f || NdL < 0.0f)
        return 0.0f;
    if(roughness <= 0.0f) {
        // Delta function
        // FIXME: Should we treat this like a mirror in this case?
        return 1.0f;
    }

    const auto H = (wi + wo).normalized();

    const float NdH = clampedDot(normal, H);
    const float VdH = clampedDot(wi, H);
    const float LdH = clampedDot(wo, H);

    // Fresnel: Fraction of light reflected
    // TODO: Derive F0 from the indices of refraction
    //float F0 = 0.04;
    //float F0 = 0.25;
    float F0 = 0.75;
    //float F0 = 1.0;
    float F = fresnelDialectricSchlick(F0, VdH);

    // Geometric attenuation according to Blinn
    float G1 = 2.0 * NdH * NdV / VdH;
    float G2 = 2.0 * NdH * NdL / LdH;
    float G = std::min(1.0f, std::min(G1, G2));

    // Beckman microfacet distribution function
    const float m = roughness;
    float D = 1.0 / (M_PI * m * m * NdH * NdH * NdH * NdH)
        * exp((NdH * NdH - 1) / (m * m * NdH * NdH));

    float denom = 4.0f * NdV * NdL;

    return F * G * D / denom;
}

