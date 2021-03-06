
#include "Material.h"
#include "Scene.h"
#include "Ray.h"
#include "AmbientOcclusionShader.h"
#include "RandomNumberGenerator.h"


void AmbientOcclusionShader::shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection )
{
    const unsigned int num_ao_rays = 8;
    unsigned int hits = 0;
    float value = 0;
    Ray ao_ray;
    RayIntersection ao_intersection;
    //ao_ray.origin = intersection.position;
    Vector4 offset( 0.0, 0.0, 0.0 );
    scale( intersection.normal, EPSILON, offset ); // NOTE - Seems to help 
    add( intersection.position, offset, ao_ray.origin );

    for( unsigned int aori = 0; aori < num_ao_rays; aori++ ) {
        rng.uniformSurfaceUnitHalfSphere( intersection.normal, ao_ray.direction );
        ao_ray.direction.makeDirection();

        ao_intersection = RayIntersection();
        ao_intersection.min_distance = EPSILON;

        // FIXME???: Can't use intersectsAny() because it will return true for
        // infinite light sources like environment maps.
        //if( scene.intersectsAny( ao_ray, ao_intersection.min_distance ) ) {
        if( scene.intersect( ao_ray, ao_intersection ) &&
            ao_intersection.distance != FLT_MAX ) {
            hits++;
        }
    }
    value = 1.0f - (float) hits / (float) num_ao_rays;

    if( intersection.material ) {
        intersection.sample.color = intersection.material->diffuse(intersection).scaled(value);
    }
    else {
        intersection.sample.color.setRGB(value, value, value);
    }
}


