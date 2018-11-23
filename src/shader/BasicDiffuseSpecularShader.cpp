
#include <cassert>
#include <iostream>
#include "Material.h"
#include "Scene.h"
#include "Ray.h"
#include "BasicDiffuseSpecularShader.h"
#include "RandomNumberGenerator.h"
#include "AxisAlignedSlab.h"
#include "EnvironmentMap.h"

void BasicDiffuseSpecularShader::shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection )
{
    RGBColor reflected_contrib( 0.0, 0.0, 0.0 );
    RGBColor direct_contrib( 0.0, 0.0, 0.0 );
    Ray new_ray;
    RayIntersection new_intersection;
    new_intersection.min_distance = EPSILON;
    Vector4 offset( 0.0, 0.0, 0.0 );
    //offset = scale( intersection.normal, EPSILON ); // NOTE - Seems to help 
    new_ray.origin = add( intersection.position, offset );
    new_ray.depth = intersection.ray.depth + 1;
    const unsigned char max_depth = 5;

    // Asserts
    intersection.normal.assertIsUnity();
    intersection.normal.assertIsDirection();

    // Direct lighting
    
    // Area lights
    if( sample_area_lights ) {
        direct_contrib += sampleAreaLights( scene, intersection, rng );
    }

    // Point lights
    direct_contrib += samplePointLights( scene, intersection );

    // Environment map
    if( sample_env_maps ) {
        direct_contrib += sampleEnvironmentMap( scene, intersection, rng );
    }

    auto & material = *intersection.material;
    const bool is_perfect = material.perfect_reflector || material.perfect_refractor;
    RGBColor Li, Lo;

    if( intersection.ray.depth < max_depth ) {
        auto sample = material.sampleBxDF( rng, intersection );
        new_ray.direction = sample.direction;

        if( is_perfect ) { new_ray.index_of_refraction = sample.new_index_of_refraction; }
        else             { new_ray.index_of_refraction = intersection.ray.index_of_refraction; }

        if( scene.intersect( new_ray, new_intersection ) ) {
            if( new_intersection.distance != FLT_MAX
                && !(sample_area_lights && new_intersection.traceable->isAreaLight()) ) {
                shade( scene, rng, new_intersection );
            }
            Li = new_intersection.sample.color;
            if( sample.pdf_sample == DistributionSample::DIRAC_PDF_SAMPLE
                || is_perfect ) {
                Lo = Li;
            }
            else {
                //Lo = reflectedRadiance( intersection, Li, sample.direction );
                // account for projected area of the surface
                float projection_cos = clampedDot( new_intersection.normal, new_ray.direction.negated() );
                Lo = projection_cos * reflectedRadiance( intersection, Li, sample.direction );
                Lo.scale( 2.0 * M_PI / sample.pdf_sample );
            }
            reflected_contrib += Lo;
        }
        else if( !sample_env_maps
                 || (scene.env_map && !scene.env_map->canImportanceSample())
                 || sample.pdf_sample == DistributionSample::DIRAC_PDF_SAMPLE
                 || is_perfect ) {
            if( scene.intersectEnvMap( new_ray, new_intersection ) ) {
                Li = new_intersection.sample.color;
                if( sample.pdf_sample == DistributionSample::DIRAC_PDF_SAMPLE
                    || is_perfect ) {
                    Lo = Li;
                }
                else {
                    Lo = reflectedRadiance( intersection, Li, sample.direction );
                    Lo.scale( 2.0 * M_PI / sample.pdf_sample );
                }
                reflected_contrib += Lo;
            }
        }
    }

    intersection.sample.color += direct_contrib + reflected_contrib + intersection.material->emittance;
}


