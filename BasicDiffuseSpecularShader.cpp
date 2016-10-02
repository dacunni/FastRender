
#include <cassert>
#include "Material.h"
#include "Scene.h"
#include "Ray.h"
#include "BasicDiffuseSpecularShader.h"
#include "RandomNumberGenerator.h"
#include "AxisAlignedSlab.h"


// TODO 
//  - diffuse
//      - trace rays to get irradiance
//      - evaluate diffuse formula
//  - specular
//      - trace specular rays
//      - evaluate specular formula

void BasicDiffuseSpecularShader::shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection )
{
    RGBColor diffuse_contrib( 0.0, 0.0, 0.0 );
    RGBColor specular_contrib( 0.0, 0.0, 0.0 );
    RGBColor direct_contrib( 0.0, 0.0, 0.0 );
    Ray new_ray;
    //RayIntersection new_intersection;
    RayIntersection new_intersection = RayIntersection();
    new_intersection.min_distance = 0.01;
    Vector4 offset( 0.0, 0.0, 0.0 );
    //offset = scale( intersection.normal, 0.01 ); // NOTE - Seems to help 
    new_ray.origin = add( intersection.position, offset );
    new_ray.depth = intersection.ray.depth + 1;
    const unsigned char max_depth = 5;
    //const unsigned char max_depth = 4;
    //const unsigned char max_depth = 3;

    Vector4 from_dir = intersection.ray.direction;
    from_dir.negate();

    // Asserts
    from_dir.assertIsUnity();
    from_dir.assertIsDirection();
    intersection.normal.assertIsUnity();
    intersection.normal.assertIsDirection();

    // Direct lighting
    //
    // Area lights
    //for( Traceable * traceable : scene.lights ) {
    //    // TODO - sample lights
    //}
    // Point lights
    for( const PointLight & light : scene.point_lights ) {
        Vector4 to_light = subtract( light.position, intersection.position );
        Vector4 direction = to_light.normalized();
        direction.makeDirection();

        // Shoot a ray toward the light to see if we are in shadow
        Ray shadow_ray( intersection.position, direction );
        if( scene.intersectsAny( shadow_ray, 0.01 ) ) {
            continue;
        }

        // Not in shadow
        float cos_r_n = fabs( dot( direction, intersection.normal ) ); 
        RGBColor color = light.band_power;
        color.scale(cos_r_n);
        color.scale(1.0f / to_light.magnitude_sq()); // distance falloff
        // TODO: use actual material parameters properly so we can get specular here, too
        if( intersection.material )
            direct_contrib.accum( mult( color, intersection.material->diffuse ) );
        else
            direct_contrib.accum( color );
    }

    if( intersection.ray.depth < max_depth ) {
        // TODO - How best should we choose between diffuse and specular?
        // TODO - should we scale the color by the inverse probability for the sampling we chose?
        const float diffuse_chance = 0.9;
        float diff_spec_select = rng.uniform01();

        if( intersection.material && intersection.material->perfect_reflector ) {
            new_ray.direction = mirror( from_dir, intersection.normal );

            if( scene.intersect( new_ray, new_intersection ) ) {
                if( new_intersection.distance != FLT_MAX ) {
                    shade( scene, rng, new_intersection );
                }
                // FIXME: Should I scale by the cosine for a perfect reflector?
                //float cos_r_n = dot( new_ray.direction, intersection.normal ); 
                //new_intersection.sample.color.scale( cos_r_n );
                specular_contrib.accum( new_intersection.sample.color );
            }
        }
        else if( intersection.material && intersection.material->perfect_refractor ) {
            float index_in = intersection.ray.index_of_refraction;
            float index_out = intersection.material->index_of_refraction;

            // FIXME: HACK - This assumes that if we hit the surface of an object with the same
            //        index of refraction as the material we're in, then we are moving back into
            //        free space. This might not be true if there are numerical errors tracing
            //        abutting objects of the same material type, or for objects that are intersecting.
            if( !intersection.material ||
                intersection.material->index_of_refraction == intersection.ray.index_of_refraction ) {
                index_out = 1.0f;
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

            // Trace refraction
            if( fresnel < 1.0f ) {
                new_ray.direction = refracted;
                new_ray.index_of_refraction = index_out;

                //printf("\trefract ior %f -> %f\n", intersection.ray.index_of_refraction, new_ray.index_of_refraction); // TEMP
                if( scene.intersect( new_ray, new_intersection ) ) {
                    if( new_intersection.distance != FLT_MAX ) {
                        shade( scene, rng, new_intersection );
                    }
                    specular_contrib.accum( new_intersection.sample.color.scaled( 1.0 - fresnel ) );
                }
            }

            // Trace reflection
            if( fresnel > 0.0f ) {
                new_ray.direction = mirror( from_dir, intersection.normal );
                new_ray.index_of_refraction = index_in;

                //printf("\treflect ior %f -> %f\n", intersection.ray.index_of_refraction, new_ray.index_of_refraction); // TEMP
                if( scene.intersect( new_ray, new_intersection ) ) {
                    if( new_intersection.distance != FLT_MAX ) {
                        shade( scene, rng, new_intersection );
                    }
                    specular_contrib.accum( new_intersection.sample.color.scaled( fresnel ) );
                }
            } 
        }
        else if( diff_spec_select < diffuse_chance ) {
            // Diffuse
            rng.uniformSurfaceUnitHalfSphere( intersection.normal, new_ray.direction );
            new_ray.direction.makeDirection();

            //new_intersection = RayIntersection();
            //new_intersection.min_distance = 0.01;
            if( scene.intersect( new_ray, new_intersection ) ) {
                if( new_intersection.distance != FLT_MAX )
                    shade( scene, rng, new_intersection );
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
                if( new_intersection.distance != FLT_MAX )
                    shade( scene, rng, new_intersection );
                float cos_r_n = dot( new_ray.direction, intersection.normal ); 
                new_intersection.sample.color.scale( cos_r_n );
                specular_contrib.accum( new_intersection.sample.color );
            }
        }
    }

    if( intersection.material ) {
        intersection.sample.color = mult( diffuse_contrib, intersection.material->diffuse );
        intersection.sample.color.accum( mult( specular_contrib, intersection.material->specular ) );
        intersection.sample.color.accum( intersection.material->emittance );
        intersection.sample.color.accum( direct_contrib );
    }
    else {
        intersection.sample.color = diffuse_contrib;
        intersection.sample.color.accum( direct_contrib );
    }
}


