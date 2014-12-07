
#include "Material.h"
#include "Scene.h"
#include "Ray.h"
#include "BasicDiffuseSpecularShader.h"
#include "RandomNumberGenerator.h"


// TODO 
//  - diffuse
//      - trace rays to get irradiance
//      - evaluate diffuse formula
//  - specular
//      - trace specular rays
//      - evaluate specular formula

void BasicDiffuseSpecularShader::shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection )
{
    const unsigned int num_diffuse_rays = 100;
    RGBColor diffuse_contrib( 0.0, 0.0, 0.0 );
    RGBColor specular_contrib( 0.0, 0.0, 0.0 );
    Ray diffuse_ray, specular_ray;
    RayIntersection diffuse_intersection;
    RayIntersection specular_intersection;
    Vector4 offset( 0.0, 0.0, 0.0 );
    scale( intersection.normal, 0.01, offset ); // NOTE - Seems to help 
    add( intersection.position, offset, diffuse_ray.origin );
    add( intersection.position, offset, specular_ray.origin );
    diffuse_ray.depth = intersection.ray.depth + 1;
    specular_ray.depth = intersection.ray.depth + 1;
    const unsigned char max_depth = 3;

    if( intersection.ray.depth < max_depth ) {
        // Diffuse
        for( unsigned int diffuseri = 0; diffuseri < num_diffuse_rays; diffuseri++ ) {
            rng.uniformSurfaceUnitHalfSphere( intersection.normal, diffuse_ray.direction );

            diffuse_intersection = RayIntersection();
            diffuse_intersection.min_distance = 0.01;
            if( scene.intersect( diffuse_ray, diffuse_intersection ) ) {
                shade( scene, rng, diffuse_intersection );
                float cos_r_n = dot( diffuse_ray.direction, intersection.normal ); 
                diffuse_intersection.sample.color.scale( cos_r_n );
                diffuse_contrib.accum( diffuse_intersection.sample.color );
            }
        }

        if( num_diffuse_rays > 0 ) {
            diffuse_contrib.scale( 1.0f / num_diffuse_rays );
        }

        // Specular
        specular_intersection = RayIntersection();
        specular_intersection.min_distance = 0.01;
        Vector4 from_dir = intersection.ray.direction;
        from_dir.negate();
        mirror( from_dir, intersection.normal, specular_ray.direction );

        if( scene.intersect( specular_ray, specular_intersection ) ) {
            shade( scene, rng, specular_intersection );
            float cos_r_n = dot( specular_ray.direction, intersection.normal ); 
            specular_intersection.sample.color.scale( cos_r_n );
            specular_contrib.accum( specular_intersection.sample.color );
        }
    }

    if( intersection.material ) {
        intersection.sample.color = mult( diffuse_contrib, intersection.material->diffuse );
        intersection.sample.color.accum( mult( specular_contrib, intersection.material->specular ) );
        intersection.sample.color.accum( intersection.material->emittance );
    }
    else {
        intersection.sample.color = diffuse_contrib;
    }
}


