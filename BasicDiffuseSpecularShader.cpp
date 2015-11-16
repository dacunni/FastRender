
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
    RayIntersection new_intersection;
    Vector4 offset( 0.0, 0.0, 0.0 );
    offset = scale( intersection.normal, 0.01 ); // NOTE - Seems to help 
    new_ray.origin = add( intersection.position, offset );
    new_ray.depth = intersection.ray.depth + 1;
    const unsigned char max_depth = 3;

    //for( Traceable * traceable : scene.lights ) {
    //    // TODO - sample lights
    //}

    if( intersection.ray.depth < max_depth ) {
        // TODO - How best should we choose between diffuse and specular?
        // TODO - should we scale the color by the inverse probability for the sampling we chose?
        const float diffuse_chance = 0.9;
        float diff_spec_select = rng.uniform01();

        if( diff_spec_select < diffuse_chance ) {
            // Diffuse
            rng.uniformSurfaceUnitHalfSphere( intersection.normal, new_ray.direction );

            new_intersection = RayIntersection();
            new_intersection.min_distance = 0.01;
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
            new_intersection = RayIntersection();
            new_intersection.min_distance = 0.01;
            Vector4 from_dir = intersection.ray.direction;
            from_dir.negate();
            new_ray.direction = mirror( from_dir, intersection.normal );

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
    }
}


