
#include <typeinfo>
#include "Material.h"
#include "RandomNumberGenerator.h"
#include "GeometryUtils.h"
#include "Fresnel.h"

std::shared_ptr<Material> DEFAULT_MATERIAL = std::make_shared<DiffuseMaterial>();

MaterialParamRGB makeConstantParamRGB(const RGBColor & c)
{
    return [c](const RayIntersection &) { return c; };
}

MaterialParamRGB makeConstantParamRGB(float r, float g, float b)
{
    return makeConstantParamRGB(RGBColor(r, g, b));
}

Material::Material()
{
    setAlbedo(makeConstantParamRGB(1, 1, 1));
}

float
Material::BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo ) const
{
    printf("BxDF is not overridden!!! (%s)\n", typeid(*this).name());
    return 1.0f / M_PI; // Perfectly diffuse
}

DistributionSample Material::sampleHemisphereUniform( RandomNumberGenerator & rng,
                                                      const RayIntersection & intersection ) const
{
    DistributionSample sample;
    rng.uniformSurfaceUnitHalfSphere( intersection.normal, sample.direction );
    //sample.pdf_sample = 1.0f / (2.0f * M_PI);
    // Special case for diffuse. reflectedRadiance() already accounts for the divide by
    // pi for diffuse, so as long as we're doing uniform sampling in sampleBxDF, we should
    // return unity here so things balance out appropriately.
    sample.pdf_sample = 1.0f;
    sample.direction.makeDirection();
    return sample;
}

DistributionSample Material::sampleCosineLobe( RandomNumberGenerator & rng,
                                               const RayIntersection & intersection ) const
{
    DistributionSample sample;

    // FIXME: There must be a faster way

    // Generate an arbitrary direction to help us make a basis for tangent space.
    // The likelyhood that this is pointing in the same direction as the normal is very low.
    Vector4 elsewhere; rng.uniformVolumeUnitCube(elsewhere);
    Vector4 tangent = cross(elsewhere, intersection.normal).normalized();
    Vector4 bitangent = cross(intersection.normal, tangent).normalized();

    // Randomly generate a direction from a cosine-weighted pdf centered about the normal
    Vector4 R; rng.cosineUnitHalfSphere(R);
    sample.direction = R.x * tangent + R.y * bitangent + R.z * intersection.normal;
    sample.direction.makeDirection();
    sample.pdf_sample = dot(sample.direction, intersection.normal) / M_PI;

    //sample.pdf_sample *= M_PI; // TEMP - FIXME - this shouldn't be here
    sample.pdf_sample *= 2.0 * M_PI; // TEMP - FIXME - this shouldn't be here

    return sample;
}

DistributionSample
Material::sampleBxDF( RandomNumberGenerator & rng,
                      const RayIntersection & intersection ) const
{
#ifdef USE_COSINE_SAMPLING
    return sampleCosineLobe(rng, intersection);
#else
    return sampleHemisphereUniform(rng, intersection);
#endif
}

float
DiffuseMaterial::BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo ) const
{
#ifdef USE_COSINE_SAMPLING
    return 1.0f / M_PI; // Perfectly diffuse
#else
    return 1.0f / (2.0f * M_PI);
#endif
}

DistributionSample
DiffuseMaterial::sampleBxDF( RandomNumberGenerator & rng,
                             const RayIntersection & intersection ) const
{
#ifdef USE_COSINE_SAMPLING
    return sampleCosineLobe(rng, intersection);
#else
    return sampleHemisphereUniform(rng, intersection);
#endif
}

DiffuseCheckerBoardMaterial::DiffuseCheckerBoardMaterial()
: DiffuseCheckerBoardMaterial(1, 1, 1, 0, 0, 0)
{ }

DiffuseCheckerBoardMaterial::DiffuseCheckerBoardMaterial(float r, float g, float b)
: DiffuseCheckerBoardMaterial(r, g, b, 0, 0, 0)
{ }

DiffuseCheckerBoardMaterial::DiffuseCheckerBoardMaterial(float r1, float g1, float b1,
                                                         float r2, float g2, float b2)
: Material()
{ 
    RGBColor color1(r1, g1, b1);
    RGBColor color2(r2, g2, b2);
    setAlbedo([color1, color2, this](const RayIntersection & isect) {
        return (bool(int(floorf(isect.position.x / gridSize)) % 2) ^
                //bool(int(floorf(isect.position.y / gridSize)) % 2) ^
                bool(int(floorf(isect.position.z / gridSize)) % 2))
            ? color1 : color2;
    });
}

float
DiffuseCheckerBoardMaterial::BxDF( const Vector4 & normal, const Vector4 & wi, const Vector4 & wo ) const
{
    return 1.0f / M_PI; // Perfectly diffuse
}

DiffuseUVMaterial::DiffuseUVMaterial() : DiffuseMaterial()
{
    setAlbedo([this](const RayIntersection & isect) {
        float u = clamp01(isect.u);
        float v = clamp01(isect.v);
        return RGBColor(u, v, 0.0f);
    });
}

DiffuseTextureMaterial::DiffuseTextureMaterial( std::shared_ptr<SurfaceTexture> & tex )
    : DiffuseMaterial(), texture(tex)
{
    setAlbedo([this](const RayIntersection & isect) {
        float u = clamp01(isect.u);
        float v = clamp01(isect.v);
        return texture->image.sampleRGB(u, v);
    });
}

DistributionSample
DiffuseCheckerBoardMaterial::sampleBxDF( RandomNumberGenerator & rng,
                                         const RayIntersection & intersection ) const
{
#ifdef USE_COSINE_SAMPLING
    return sampleCosineLobe(rng, intersection);
#else
    return sampleHemisphereUniform(rng, intersection);
#endif
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
        fresnel = Fresnel::Dialectric::Unpolarized( dot( from_dir, intersection.normal ),
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

