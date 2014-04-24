
#include "Material.h"
#include "Scene.h"
#include "Ray.h"
#include "AmbientOcclusionShader.h"
#include "RandomNumberGenerator.h"


void AmbientOcclusionShader::shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection )
{
    const unsigned int num_ao_rays = 32;
    unsigned int hits = 0;
    float value = 0;
    Ray ao_ray;
    RayIntersection ao_intersection;
    //ao_ray.origin = intersection.position;
    Vector4 offset( 0.0, 0.0, 0.0 );
    scale( intersection.normal, 0.01, offset ); // NOTE - Seems to help 
    add( intersection.position, offset, ao_ray.origin );

    for( unsigned int aori = 0; aori < num_ao_rays; aori++ ) {
        rng.uniformSurfaceUnitHalfSphere( intersection.normal, ao_ray.direction );

        ao_intersection = RayIntersection();
        ao_intersection.min_distance = 0.01;
        if( scene.intersectsAny( ao_ray, ao_intersection.min_distance ) ) {
            hits++;
        }
    }
    value = 1.0f - (float) hits / (float) num_ao_rays;

    if( intersection.material ) {
        intersection.sample.color.r = value * intersection.material->diffuse.r;
        intersection.sample.color.g = value * intersection.material->diffuse.g;
        intersection.sample.color.b = value * intersection.material->diffuse.b;
    }
    else {
        intersection.sample.color.r = value;
        intersection.sample.color.g = value;
        intersection.sample.color.b = value;
    }
}


