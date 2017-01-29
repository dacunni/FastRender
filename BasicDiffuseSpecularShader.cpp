
#include <cassert>
#include <iostream>
#include "Material.h"
#include "Scene.h"
#include "Ray.h"
#include "BasicDiffuseSpecularShader.h"
#include "RandomNumberGenerator.h"
#include "AxisAlignedSlab.h"


void BasicDiffuseSpecularShader::shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection )
{
    RGBColor diffuse_contrib( 0.0, 0.0, 0.0 );
    RGBColor specular_contrib( 0.0, 0.0, 0.0 );
    RGBColor direct_contrib( 0.0, 0.0, 0.0 );
    Ray new_ray;
    RayIntersection new_intersection;
    new_intersection.min_distance = 0.01;
    Vector4 offset( 0.0, 0.0, 0.0 );
    //offset = scale( intersection.normal, 0.01 ); // NOTE - Seems to help 
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
            shadow_isect.min_distance = 0.01;
            if( scene.intersect( shadow_ray, shadow_isect ) ) {
                float dist_sq_to_isect = sq(shadow_isect.distance + shadow_isect.min_distance);
                if( dist_sq_to_isect < dist_sq_to_light) {
                    continue;
                }
            }

            // Not in shadow
            float cos_r_n = fabs( dot( direction, intersection.normal ) ); 
            RGBColor color = light->material->emittance;
            color.scale( cos_r_n );
            color.scale( fabs( dot( sample.normal, direction.negated() ) ) ); // account for projected area of the light
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
        shadow_isect.min_distance = 0.01;
        if( scene.intersect( shadow_ray, shadow_isect )
            && sq(shadow_isect.distance) < dist_sq_to_light ) {
            continue;
        }

        // Not in shadow
        float cos_r_n = fabs( dot( direction, intersection.normal ) ); 
        RGBColor color = light.band_power;
        color.scale(cos_r_n);
        color.scale(1.0f / to_light.magnitude_sq()); // distance falloff
        // TODO: use actual material parameters properly so we can get specular here, too
        if( intersection.material )
            direct_contrib.accum( mult( color, intersection.material->diffuse(intersection) ) );
        else
            direct_contrib.accum( color );
    }

    // TODO - How best should we choose between diffuse and specular?
    // FIXME: Should I scale by the cosine for a perfect reflector?

    if( intersection.ray.depth < max_depth ) {
        const float diffuse_chance = 0.9;
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
        }
        else if( diff_spec_select < diffuse_chance ) {
            // Diffuse
            rng.uniformSurfaceUnitHalfSphere( intersection.normal, new_ray.direction );
            new_ray.direction.makeDirection();

            if( scene.intersect( new_ray, new_intersection ) ) {
                if( new_intersection.distance != FLT_MAX
                    && !(sample_area_lights && new_intersection.traceable->isAreaLight())
                  ) {
                    shade( scene, rng, new_intersection );
                }
                float cos_r_n = dot( new_ray.direction, intersection.normal ); 
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
                    && !new_intersection.material->isEmitter()
                    ) {
                    shade( scene, rng, new_intersection );
                }
                float cos_r_n = dot( new_ray.direction, intersection.normal ); 
                new_intersection.sample.color.scale( cos_r_n );
                specular_contrib.accum( new_intersection.sample.color );
            }
        }
    }

    if( intersection.material ) {
        intersection.sample.color = mult( diffuse_contrib, intersection.material->diffuse(intersection) );
        intersection.sample.color.accum( mult( specular_contrib, intersection.material->specular(intersection) ) );
        intersection.sample.color.accum( intersection.material->emittance );
        intersection.sample.color.accum( direct_contrib );
    }
    else {
        intersection.sample.color = diffuse_contrib;
        intersection.sample.color.accum( direct_contrib );
    }
}


