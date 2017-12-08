
#include "Material.h"
#include "RandomNumberGenerator.h"
#include "GeometryUtils.h"

std::shared_ptr<Material> DEFAULT_MATERIAL = std::make_shared<DiffuseMaterial>();

float
Material::BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo )
{
    printf("BxDF is not overridden!!!\n");
    return 1.0f / M_PI; // Perfectly diffuse
}

DistributionSample
Material::sampleBxDF( RandomNumberGenerator & rng,
                      const RayIntersection & intersection ) const
{
    // Perfectly diffuse surface
    DistributionSample sample;
    rng.uniformSurfaceUnitHalfSphere( intersection.normal, sample.direction );
    sample.direction.makeDirection();
    sample.pdf_sample = 1.0f / (2.0f * M_PI);
    return sample;
}

DistributionSample
DiffuseMaterial::sampleBxDF( RandomNumberGenerator & rng,
                             const RayIntersection & intersection ) const
{
    // Perfectly diffuse surface
    DistributionSample sample;
    rng.uniformSurfaceUnitHalfSphere( intersection.normal, sample.direction );
    sample.direction.makeDirection();
    sample.pdf_sample = 1.0f / (2.0f * M_PI);
    return sample;
}

float
DiffuseMaterial::BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo )
{
    return 1.0f / M_PI; // Perfectly diffuse
}

RGBColor
DiffuseCheckerBoardMaterial::diffuse( const RayIntersection & isect ) {
    return (bool(int(floorf(isect.position.x / gridSize)) % 2) ^
            bool(int(floorf(isect.position.y / gridSize)) % 2) ^
            bool(int(floorf(isect.position.z / gridSize)) % 2)
            )
        ?  diffuseColor 
        : RGBColor(0.0f, 0.0f, 0.0f);
        //?  RGBColor(1.0f, 0.0f, 0.0f)
        //: RGBColor(0.0f, 1.0f, 0.0f);
}

float
DiffuseCheckerBoardMaterial::BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo )
{
    return 1.0f / M_PI; // Perfectly diffuse
}

RGBColor
DiffuseUVMaterial::diffuse( const RayIntersection & isect )
{
    float u = clamp01(isect.u);
    float v = clamp01(isect.v);
    return RGBColor(u, v, 0.0f);
}

RGBColor
DiffuseTextureMaterial::diffuse( const RayIntersection & isect )
{
    float u = clamp01(isect.u);
    float v = clamp01(isect.v);
    return texture->image.sampleRGB(u, v);
}

DistributionSample
DiffuseCheckerBoardMaterial::sampleBxDF( RandomNumberGenerator & rng,
                             const RayIntersection & intersection ) const
{
    // Perfectly diffuse surface
    DistributionSample sample;
    rng.uniformSurfaceUnitHalfSphere( intersection.normal, sample.direction );
    sample.direction.makeDirection();
    sample.pdf_sample = 1.0f / (2.0f * M_PI);
    return sample;
}

DistributionSample
MirrorMaterial::sampleBxDF( RandomNumberGenerator & rng,
                            const RayIntersection & intersection ) const
{
    // Perfect mirror - PDF if a Dirac delta function
    DistributionSample sample;
    sample.direction = mirror( intersection.fromDirection(),
                               intersection.normal );
    sample.pdf_sample = DistributionSample::DIRAC_PDF_SAMPLE;
    return sample;
}

DistributionSample
RefractiveMaterial::sampleBxDF( RandomNumberGenerator & rng,
                                const RayIntersection & intersection ) const
{
    // Perfect Fresnel refractor
    DistributionSample sample;

    const float index_in = intersection.ray.index_of_refraction;
    const float index_out = index_of_refraction;
    const Vector4 from_dir = intersection.fromDirection();

    // FIXME: HACK - This assumes that if we hit the surface of an object with the same
    //        index of refraction as the material we're in, then we are moving back into
    //        free space. This might not be true if there are numerical errors tracing
    //        abutting objects of the same material type, or for objects that are intersecting.
    if( index_out == index_in ) {
        sample.new_index_of_refraction = 1.0f;
    }

    Vector4 refracted = refract( from_dir, intersection.normal, index_in, index_out );
    float fresnel = 1.0f; // default to total internal reflection if refract() returns
    // a zero length vector
    if( refracted.magnitude() > 0.0001 ) {
        fresnel = fresnelDialectric( dot( from_dir, intersection.normal ),
                                     dot( refracted, intersection.normal.negated() ),
                                     index_in,
                                     index_out );
    }

    const float draw = rng.uniform01();

    // Use RNG to choose whether to reflect or refract based on Fresnel coefficient.
    //   Random selection accounts for fresnel or 1-fresnel scale factor.
    if( fresnel == 1.0f || draw < fresnel ) {
        // Trace reflection (mirror ray scaled by fresnel or 1 for total internal reflection)
        sample.direction = mirror( from_dir, intersection.normal );
        // FIXME - How do we determine pdf_sample for total internal reflection?
        sample.pdf_sample = fresnel;
        sample.new_index_of_refraction = index_in;
    }
    else {
        // Trace refraction (refracted ray scaled by 1-fresnel)
        sample.direction = refracted;
        sample.pdf_sample = 1.0 - fresnel;
        sample.new_index_of_refraction = index_out;
    }

    return sample;
}

float
CookTorranceMaterial::BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo )
{
    const auto H = (wi + wo).normalized();

    const float NdH = clampedDot(normal, H);
    const float NdV = clampedDot(normal, wi);
    const float NdL = clampedDot(normal, wo);
    const float VdH = clampedDot(wi, H);
    const float LdH = clampedDot(wo, H);

    // Fresnel: Fraction of light reflected
    // TODO: Derive F0 from the indices of refraction
    float F0 = 0.25;
    float F = F0 + (1.0 - F0) * pow(1.0 - VdH, 5);

    // Geometric attenuation according to Blinn
    float G1 = 2.0 * NdH * NdV / VdH;
    float G2 = 2.0 * NdH * NdL / LdH;
    float G = std::min(1.0f, std::min(G1, G2));

    // Beckman microfacet distribution function
    const float & m = roughness;
    float D = 1.0 / (M_PI * m * m * NdH * NdH * NdH * NdH)
        * exp((NdH * NdH - 1) / (m * m * NdH * NdH));

    float denom = 4.0f * dot(wi, normal) * dot(wo, normal);

    return F * G * D / denom;
}

