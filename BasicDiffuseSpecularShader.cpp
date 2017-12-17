
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

    Vector4 from_dir = intersection.fromDirection();

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
        //const float diffuse_chance = 0.9; // FIXME: HACKHACK
        const float diffuse_chance = 1.0f; // FIXME: HACKHACK
        float diff_spec_select = rng.uniform01();

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
        else if( diff_spec_select <= diffuse_chance ) {
            // Diffuse
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
                Lo.scale(1.0f/sample.pdf_sample);
                diffuse_contrib.accum( Lo );
            }
            else if( !sample_env_maps
                     || (scene.env_map && !scene.env_map->canImportanceSample()) )
            {
                if( scene.intersectEnvMap( new_ray, new_intersection ) ) {
                    // FIXME: Make this match importance sampled version
                    float cos_r_n = dot( new_ray.direction, intersection.normal ); 
                    //new_intersection.sample.color.scale( 1.0f / sample.pdf_sample );
                    new_intersection.sample.color.scale( cos_r_n );
                    diffuse_contrib.accum( new_intersection.sample.color );
                }
            }
        }
        else {
            // Specular
            // TODO - sample the specular lobe, not just the mirror direction
            new_ray.direction = mirror( from_dir, intersection.normal );
            new_ray.direction.makeDirection();

            if( scene.intersect( new_ray, new_intersection ) ) {
                if( new_intersection.distance != FLT_MAX
                    //&& !new_intersection.material->isEmitter()
                    && !(sample_area_lights && new_intersection.traceable->isAreaLight())
                    ) {
                    shade( scene, rng, new_intersection );
                }
                float cos_r_n = dot( new_ray.direction, intersection.normal ); 
                new_intersection.sample.color.scale( cos_r_n );
                specular_contrib.accum( new_intersection.sample.color );
            }
            else if( scene.intersectEnvMap( new_ray, new_intersection ) ) {
                // FIXME
                specular_contrib.accum( new_intersection.sample.color );
            }
        }
    }

    intersection.sample.color = diffuse_contrib;
    intersection.sample.color += specular_contrib;
    intersection.sample.color += intersection.material->emittance;
    intersection.sample.color += direct_contrib;
}


