
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

    // Direct lighting
    //
    // Area lights
    if( sample_area_lights ) {
        for( const auto light : scene.area_lights ) {
            auto sample = light->sampleSurfaceTransformed( rng );
            auto to_light = subtract( sample.position, intersection.position );

            if( dot( to_light, intersection.normal ) <= 0.0 ) {
                continue;
            }
            
            float dist_sq_to_light = to_light.magnitude_sq();
            auto direction = to_light.normalized();
            direction.makeDirection();

            // Shoot a ray toward the light to see if we are in shadow
            Ray shadow_ray( intersection.position, direction );
            RayIntersection shadow_isect;
            shadow_isect.min_distance = EPSILON;
            if( scene.intersect( shadow_ray, shadow_isect ) ) {
                float dist_sq_to_isect = sq(shadow_isect.distance + shadow_isect.min_distance);
                if( dist_sq_to_isect < dist_sq_to_light) {
                    continue;
                }
            }

            // Not in shadow
            float cos_r_n = fabsf( dot( direction, intersection.normal ) ); 
            RGBColor color = light->material->emittance;
            color.scale( cos_r_n );
            color.scale( fabsf( dot( sample.normal, direction.negated() ) ) ); // account for projected area of the light
            // FIXME - Should there be a distance falloff? Things seem too bright for objects close to lights if we do
            //color.scale( 1.0f / dist_sq_to_light ); // distance falloff
            // TODO: use actual material parameters properly so we can get specular here, too
            direct_contrib.accum( mult( color, intersection.material->diffuse(intersection) ) );
        }
    }
    // Point lights
    for( const PointLight & light : scene.point_lights ) {
        Vector4 to_light = subtract( light.position, intersection.position );
        float dist_sq_to_light = to_light.magnitude_sq();
        Vector4 direction = to_light.normalized();
        direction.makeDirection();

        // Shoot a ray toward the light to see if we are in shadow
        Ray shadow_ray( intersection.position, direction );
        RayIntersection shadow_isect;
        shadow_isect.min_distance = EPSILON;
        if( scene.intersect( shadow_ray, shadow_isect )
            && sq(shadow_isect.distance) < dist_sq_to_light ) {
            continue;
        }

        // Not in shadow
        float cos_r_n = fabsf( dot( direction, intersection.normal ) ); 
        RGBColor color = light.band_power;
        color.scale(cos_r_n);
        color.scale(1.0f / to_light.magnitude_sq()); // distance falloff
        // TODO: use actual material parameters properly so we can get specular here, too
        direct_contrib.accum( mult( color, intersection.material->diffuse(intersection) ) );
    }
    // Environment map
    if( scene.env_map && scene.env_map->canImportanceSample() ) {
        EnvironmentMap::ImportanceSample isamp;

        // Find a sample in the direction of the normal
        float s_dot_n = -1.0;
        const unsigned int max_tries = 100;
        for( unsigned int tries = 0; tries < max_tries && s_dot_n < 0.0f; tries++ ) {
            isamp = scene.env_map->importanceSample( rng, intersection );
            s_dot_n = dot( isamp.ray.direction, intersection.normal);
        }

        // Shoot a ray toward the light to see if we are in shadow
        Ray shadow_ray = isamp.ray;
        RayIntersection shadow_isect;
        shadow_isect.min_distance = EPSILON;
        if( s_dot_n > 0.0f &&
            !scene.intersect( shadow_ray, shadow_isect ) ) {
            // Not in shadow
            auto sample = scene.env_map->sample(isamp.ray);
            float cos_r_n = fabsf( dot( isamp.ray.direction, intersection.normal ) ); 
            auto color = sample.color;
            color.scale(1.0f / isamp.pdf);
            color.scale(cos_r_n);
            //color.scale(0.01f); // TEMP
            //color.scale(4.0f); // TEMP - brightening it to see better
            //color.scale(2.0f*M_PI); // FIXME: Is this right?
            //color.scale(M_PI); // FIXME: Is this right?
            direct_contrib.accum( mult( color, intersection.material->diffuse(intersection) ) );
        }
    }

    // TODO: How best should we choose between diffuse and specular?
    // FIXME: Should I scale by the cosine for a perfect reflector?
    // TODO: Generalize BRDF sampling
    // TODO: Generalize BRDF importance sampling
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
            rng.uniformSurfaceUnitHalfSphere( intersection.normal, new_ray.direction );
            new_ray.direction.makeDirection();
            const float pdf = 1.0f / (2.0f * M_PI);

            if( scene.intersect( new_ray, new_intersection ) ) {
                if( new_intersection.distance != FLT_MAX
                    && !(sample_area_lights && new_intersection.traceable->isAreaLight()) )
                {
                    shade( scene, rng, new_intersection );
                }
                float cos_r_n = dot( new_ray.direction, intersection.normal ); 
                new_intersection.sample.color.scale( cos_r_n );
                diffuse_contrib.accum( new_intersection.sample.color );
            }
            else if( scene.env_map && !scene.env_map->canImportanceSample()
                     && scene.intersectEnvMap( new_ray, new_intersection ) )
            {
                // FIXME: Make this match importance sampled version
                float cos_r_n = dot( new_ray.direction, intersection.normal ); 
                new_intersection.sample.color.scale( 1.0f / pdf );
                new_intersection.sample.color.scale( cos_r_n );
                diffuse_contrib.accum( new_intersection.sample.color );
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

    intersection.sample.color = mult( diffuse_contrib, intersection.material->diffuse(intersection) );
    intersection.sample.color.accum( mult( specular_contrib, intersection.material->specular(intersection) ) );
    intersection.sample.color.accum( intersection.material->emittance );
    intersection.sample.color.accum( direct_contrib );
}


