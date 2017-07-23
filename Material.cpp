
#include "Material.h"
#include "RandomNumberGenerator.h"
#include "GeometryUtils.h"

std::shared_ptr<Material> DEFAULT_MATERIAL = std::make_shared<DiffuseMaterial>();

float
Material::BxDF( const RayIntersection & intersection ) const
{
    printf("BxDF is not overridden!!!\n");
    return 1.0f; // Perfectly diffuse
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
DiffuseMaterial::BxDF( const RayIntersection & intersection ) const
{
    return 1.0f; // Perfectly diffuse
}

RGBColor
DiffuseCheckerBoardMaterial::diffuse( RayIntersection & isect ) {
    return (bool(int(floorf(isect.position.x / gridSize)) % 2) ^
            bool(int(floorf(isect.position.y / gridSize)) % 2) ^
            bool(int(floorf(isect.position.z / gridSize)) % 2)
            )
        ?  diffuseColor 
        : RGBColor(0.0f, 0.0f, 0.0f);
        //?  RGBColor(1.0f, 0.0f, 0.0f)
        //: RGBColor(0.0f, 1.0f, 0.0f);
}

RGBColor
DiffuseUVMaterial::diffuse( RayIntersection & isect )
{
    float u = clamp01(isect.u);
    float v = clamp01(isect.v);
    return RGBColor(u, v, 0.0f);
}

RGBColor
DiffuseTextureMaterial::diffuse( RayIntersection & isect )
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







