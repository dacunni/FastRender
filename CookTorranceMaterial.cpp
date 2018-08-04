
#include <typeinfo>
#include "CookTorranceMaterial.h"
#include "RandomNumberGenerator.h"
#include "GeometryUtils.h"
#include "Microfacet.h"

static const float DEFAULT_ROUGHNESS = 0.1;

// Roughness less than this value is treated as perfectly smooth.
// This prevents numerical issues at or near roughness of 0.
static const float MIN_ROUGHNESS = 0.01f;

CookTorranceMaterial::CookTorranceMaterial()
    : CookTorranceMaterial(1, 1, 1, DEFAULT_ROUGHNESS)
{ }

CookTorranceMaterial::CookTorranceMaterial(float r, float g, float b)
    : CookTorranceMaterial(r, g, b, DEFAULT_ROUGHNESS)
{ }

CookTorranceMaterial::CookTorranceMaterial(float r, float g, float b, float _roughness)
  : Material(),
    roughness(_roughness)
{ 
    setAlbedo(makeConstantParamRGB(r, g, b));
    emittance.setRGB( 0.0f, 0.0f, 0.0f );
}

DistributionSample
CookTorranceMaterial::sampleBxDF(RandomNumberGenerator & rng,
                                 const RayIntersection & intersection) const
{
    DistributionSample sample;
    if(roughness < MIN_ROUGHNESS) {
        // Perfect mirror - PDF if a Dirac delta function
        sample.direction = mirror( intersection.fromDirection(), intersection.normal );
        sample.direction.makeDirection();
        sample.pdf_sample = DistributionSample::DIRAC_PDF_SAMPLE;
        return sample;
    }
    // TODO: Sample the distribution. This is sampling like a perfectly diffuse distribution
    return sampleCosineLobe(rng, intersection);
}

float
CookTorranceMaterial::BxDF(const Vector4 & normal, const Vector4 & wi, const Vector4 & wo) const
{
    const float NdV = dot(normal, wi);
    const float NdL = dot(normal, wo);

    // Make sure wi/wo are on the same side as the normal
    if(NdV < 0.0f || NdL < 0.0f)
        return 0.0f;

    const auto H = (wi + wo).normalized();
    const float NdH = clampedDot(normal, H);
    const float VdH = clampedDot(wi, H);

    // Fresnel: Fraction of light reflected
    // TODO: Derive F0 from the indices of refraction
    // TODO: Use proper formulation of F for conductors
    float F = fresnelDialectricSchlick(F0(), VdH);

    // Geometric shadowing
    float G = GeometryShadowing::Implicit(NdV, NdL);

    // Normal distribution function
    float D = roughness < MIN_ROUGHNESS
        ? 0.0f : NormalDistribution::BlinnPhong(roughness, NdH);

    float bxdf = F * G * D / (4.0f * NdV * NdL);

    assert(std::isfinite(bxdf));

    //printf("F %f G %f D %f BXDF %f\n", F, G, D, bxdf);
    //printf("F %f G %f D %f BXDF %f dot(wi,wo) %f NdV %f NdL %f NdH %f \n", F, G, D, bxdf,
    //       dot(wi, wo), NdV, NdL, NdH); // TEMP

    return bxdf;
}

