
#include "Material.h"
#include "Scene.h"
#include "Ray.h"
#include "InspectionShader.h"
#include "RandomNumberGenerator.h"


void InspectionShader::shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection )
{
    RGBColor color( 0.0f, 0.0f, 0.0f );
    const float index_in = intersection.ray.index_of_refraction;
    const float index_out = intersection.material->index_of_refraction;
    const Vector4 from_dir = intersection.fromDirection();

    switch( property ) {
        case FresnelDialectric:
            {
                Vector4 refracted = refract( from_dir, intersection.normal, index_in, index_out );
                float fresnel = 1.0f;
                if( refracted.magnitude() > 0.0001 ) {
                    fresnel = fresnelDialectric( dot( from_dir, intersection.normal ),
                                                 dot( refracted, intersection.normal.negated() ),
                                                 index_in,
                                                 index_out );
                }
                color = RGBColor( fresnel, fresnel, fresnel );
            }
            break;
        case FresnelConductor:
            {
                float absorptionCoeff = 2.0; // FIXME: material specific
                float fresnel = fresnelConductor( dot( from_dir, intersection.normal ), index_out, absorptionCoeff );
                color = RGBColor( fresnel, fresnel, fresnel );
            }
            break;
        case Normal:
            {
                auto shifted = add( scale( intersection.normal, 0.5 ),
                                    Vector4( 0.5, 0.5, 0.5 ) );
                color = RGBColor( shifted.x, shifted.y, shifted.z );
            }
            break;
        case IndexOfRefraction:
            {
                float v = index_out / 3.0;
                color = RGBColor( v, v, v );
            }
            break;
        case TextureUVCoordinate:
            {
                color = RGBColor( intersection.u, intersection.v, 0.0 );
            }
            break;
        default:
            ;
    }

    intersection.sample.color = color;
}


