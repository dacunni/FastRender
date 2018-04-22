
#include <typeinfo>
#include "CookTorranceMaterial.h"
#include "RandomNumberGenerator.h"
#include "GeometryUtils.h"

static float DEFAULT_ROUGHNESS = 0.1;
static float DEFAULT_SPECULAR = 0.2;
//static float DEFAULT_SPECULAR = 1.0; // TEMP

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
    if(roughness < 0.001f) {
        // TODO: Do we need to handle 0 roughness like a mirror?
        // Perfect mirror - PDF if a Dirac delta function
        sample.direction = mirror( intersection.fromDirection(), intersection.normal );
        sample.direction.makeDirection();
        sample.pdf_sample = DistributionSample::DIRAC_PDF_SAMPLE;
        return sample;
    }
#endif
    // TODO: Sample the distribution. This is sampling like a perfectly diffuse distribution
#ifdef USE_COSINE_SAMPLING
    return sampleCosineLobe(rng, intersection);
#else
    return sampleHemisphereUniform(rng, intersection);
#endif
    return sample;
}

float cookTorranceGeometricShadowing(float NdH, float NdV, float NdL, float VdH)
{
    float G1 = 2.0f * NdH * NdV / VdH;
    float G2 = 2.0f * NdH * NdL / VdH;
    return std::min(1.0f, std::min(G1, G2));
}

float beckmanNormalDistribution(float roughness, float NdH)
{
    float m2 = roughness * roughness;
    float NdH2 = NdH * NdH;
    float NdH4 = NdH2 * NdH2;

    assert(roughness > 0.0f && m2 > 0.0f && NdH2 > 0.0f && NdH4 > 0.0f);

    float num = std::exp((NdH2 - 1) / (m2 * NdH2));
    float den = M_PI * m2 * NdH4;

    return num / den;
}

float
CookTorranceMaterial::BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo ) const
{
    const float NdV = dot(normal, wi);
    const float NdL = dot(normal, wo);

    // Make sure wi/wo are on the same side as the normal
    if(NdV < 0.0f || NdL < 0.0f)
        return 0.0f;

    if(roughness <= 0.001f) {
        // FIXME: Figure out what to return here so the specular
        //        component behaves like a mirror. For now, just
        //        return 0 so we don't get NaNs below
        return 0.0f;
    }

    const auto H = (wi + wo).normalized();
    const float NdH = clampedDot(normal, H);
    const float VdH = clampedDot(wi, H);

    // Fresnel: Fraction of light reflected
    // TODO: Derive F0 from the indices of refraction
    // TODO: Use proper formulation of F for conductors
    //float F0 = 0.04;
    //float F0 = 0.25;
    float F0 = 0.75;
    //float F0 = 0.96;
    //float F0 = 1.0;
    float F = fresnelDialectricSchlick(F0, VdH);

    // Geometric shadowing
    float G = cookTorranceGeometricShadowing(NdH, NdV, NdL, VdH);

    // Normal distribution function
    float D = beckmanNormalDistribution(roughness, NdH);

    return F * G * D / (4.0f * NdV * NdL);
}

