
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
    RGBColor diffuse_contrib( 0.0, 0.0, 0.0 );
    RGBColor specular_contrib( 0.0, 0.0, 0.0 );
    RGBColor direct_contrib( 0.0, 0.0, 0.0 );
    Ray new_ray;
    RayIntersection new_intersection;
    new_intersection.min_distance = EPSILON;
    Vector4 offset( 0.0, 0.0, 0.0 );
    //offset = scale( intersection.normal, EPSILON ); // NOTE - Seems to help 
    new_ray.origin = add( intersection.position, offset );
    new_ray.depth = intersection.ray.depth + 1;
    const unsigned char max_depth = 5;
    //const unsigned char max_depth = 4;
    //const unsigned char max_depth = 3;
    //const unsigned char max_depth = 2;

    // Asserts
    intersection.normal.assertIsUnity();
    intersection.normal.assertIsDirection();

    const bool sample_area_lights = true;
    const bool sample_env_maps = true;

    // Direct lighting
    
    // Area lights
    if( sample_area_lights ) {
        direct_contrib.accum( sampleAreaLights( scene, intersection, rng ) );
    }

    // Point lights
    direct_contrib.accum( samplePointLights( scene, intersection ) );

    // Environment map
    if( sample_env_maps ) {
        direct_contrib.accum( sampleEnvironmentMap( scene, intersection, rng ) );
    }

    // TODO: Generalize environment map intersection / sampling to generic BRDF

    if( intersection.ray.depth < max_depth ) {
        if( intersection.material->perfect_reflector
            || intersection.material->perfect_refractor ) {
            auto sample = intersection.material->sampleBxDF( rng, intersection );
            new_ray.direction = sample.direction;
            new_ray.index_of_refraction = sample.new_index_of_refraction;

            if( scene.intersect( new_ray, new_intersection ) ) {
                if( new_intersection.distance != FLT_MAX ) {
                    shade( scene, rng, new_intersection );
                }
                specular_contrib.accum( new_intersection.sample.color );
            }
            else if( scene.intersectEnvMap( new_ray, new_intersection ) ) {
                specular_contrib.accum( new_intersection.sample.color );
            }
        }
        else { // General BxDF
            auto sample = intersection.material->sampleBxDF( rng, intersection );
            new_ray.direction = sample.direction;
            new_ray.index_of_refraction = intersection.ray.index_of_refraction;

            if( scene.intersect( new_ray, new_intersection ) ) {
                if( new_intersection.distance != FLT_MAX
                    && !(sample_area_lights && new_intersection.traceable->isAreaLight()) )
                {
                    shade( scene, rng, new_intersection );
                }
                RGBColor Li = new_intersection.sample.color;
                RGBColor Lo = reflectedRadiance( intersection, Li, sample.direction );
                if(sample.pdf_sample == DistributionSample::DIRAC_PDF_SAMPLE) {
                    Lo = Li;
                }
                else {
                    Lo.scale(1.0f / sample.pdf_sample);
                    Lo.scale(2.0 * M_PI); // FIXME: This seems to be necessary
                }
                diffuse_contrib.accum( Lo );
            }
            else if( !sample_env_maps
                     || (scene.env_map && !scene.env_map->canImportanceSample())
                     || sample.pdf_sample == DistributionSample::DIRAC_PDF_SAMPLE
                     )
            {
                if( scene.intersectEnvMap( new_ray, new_intersection ) ) {
                    RGBColor Li = new_intersection.sample.color;
                    RGBColor Lo = reflectedRadiance( intersection, Li, sample.direction );
                    if(sample.pdf_sample == DistributionSample::DIRAC_PDF_SAMPLE) {
                        Lo = Li;
                    }
                    else {
                        Lo.scale(1.0f / sample.pdf_sample);
                        Lo.scale(2.0 * M_PI); // FIXME: This seems to be necessary
                    }
                    diffuse_contrib.accum( Lo );
                }
            }
        }
    }

    intersection.sample.color = diffuse_contrib;
    intersection.sample.color += specular_contrib;
    intersection.sample.color += intersection.material->emittance;
    intersection.sample.color += direct_contrib;
}


