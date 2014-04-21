
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
    const unsigned int num_diffuse_rays = 320;
    unsigned int hits = 0;
    RGBColor diffuse_contrib( 0.0, 0.0, 0.0 );
    Ray diffuse_ray;
    RayIntersection diffuse_intersection;
    //diffuse_ray.origin = intersection.position;
    Vector4 offset( 0.0, 0.0, 0.0 );
    scale( intersection.normal, 0.01, offset ); // NOTE - Seems to help 
    add( intersection.position, offset, diffuse_ray.origin );
    diffuse_ray.depth = intersection.ray.depth + 1;
    const unsigned char max_depth = 2;

    if( intersection.ray.depth < max_depth ) {
        for( unsigned int diffuseri = 0; diffuseri < num_diffuse_rays; diffuseri++ ) {
            rng.uniformSurfaceUnitHalfSphere( intersection.normal, diffuse_ray.direction );

            diffuse_intersection = RayIntersection();
            diffuse_intersection.min_distance = 0.01;
            if( scene.intersect( diffuse_ray, diffuse_intersection ) ) {
                hits++;
#if 1
                shade( scene, rng, diffuse_intersection );
                float cos_r_n = dot( diffuse_ray.direction, intersection.normal ); 

                // HACK - TODO - recursive evaluation instead of this
                if( diffuse_intersection.material ) {
                    diffuse_contrib.r += diffuse_intersection.material->emittance.r * cos_r_n;
                    diffuse_contrib.g += diffuse_intersection.material->emittance.g * cos_r_n;
                    diffuse_contrib.b += diffuse_intersection.material->emittance.b * cos_r_n;
                }
                else {
                    diffuse_contrib.r += 0.0f;
                    diffuse_contrib.g += 0.0f;
                    diffuse_contrib.b += 0.0f;
                }

                diffuse_contrib.r += diffuse_intersection.sample.color.r * cos_r_n;
                diffuse_contrib.g += diffuse_intersection.sample.color.g * cos_r_n;
                diffuse_contrib.b += diffuse_intersection.sample.color.b * cos_r_n;
                
#else
                // HACK - TODO - recursive evaluation instead of this
                if( diffuse_intersection.material ) {
                    //if( diffuse_intersection.material->emittance.r > 0.01 )
                    //    printf("%f\n", diffuse_intersection.material->emittance.r);
                    float cos_r_n = dot( diffuse_ray.direction, intersection.normal ); 
                    diffuse_contrib.r += diffuse_intersection.material->emittance.r * cos_r_n;
                    diffuse_contrib.g += diffuse_intersection.material->emittance.g * cos_r_n;
                    diffuse_contrib.b += diffuse_intersection.material->emittance.b * cos_r_n;
                }
                else {
                    diffuse_contrib.r += 0.0f;
                    diffuse_contrib.g += 0.0f;
                    diffuse_contrib.b += 0.0f;
                }
#endif
            }
        }
    }

    if( num_diffuse_rays > 0 ) {
        diffuse_contrib.r /= (float) num_diffuse_rays;
        diffuse_contrib.g /= (float) num_diffuse_rays;
        diffuse_contrib.b /= (float) num_diffuse_rays;
    }

    if( intersection.material ) {
        intersection.sample.color.r = diffuse_contrib.r * intersection.material->diffuse.r;
        intersection.sample.color.g = diffuse_contrib.g * intersection.material->diffuse.g;
        intersection.sample.color.b = diffuse_contrib.b * intersection.material->diffuse.b;
        intersection.sample.color.r += intersection.material->emittance.r;
        intersection.sample.color.g += intersection.material->emittance.g;
        intersection.sample.color.b += intersection.material->emittance.b;
    }
    else {
        intersection.sample.color.r = diffuse_contrib.r;
        intersection.sample.color.g = diffuse_contrib.g;
        intersection.sample.color.b = diffuse_contrib.b;
    }
}


