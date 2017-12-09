#include "Shader.h"
#include "Lights.h"
#include "EnvironmentMap.h"
#include "Scene.h"
#include "Vector.h"

bool Shader::inShadow( Scene & scene,
                       RayIntersection & intersection,
                       Vector4 & toLight )
{
    float dist_sq_to_light = toLight.magnitude_sq();
    Vector4 direction = toLight.normalized();
    direction.makeDirection();

    // Shoot a ray toward the light to see if we are in shadow
    Ray shadow_ray( intersection.position, direction );
    RayIntersection shadow_isect;
    shadow_isect.min_distance = EPSILON;
    return scene.intersect( shadow_ray, shadow_isect )
        && sq(shadow_isect.distance) < dist_sq_to_light;
}

RGBColor Shader::samplePointLight( const Scene & scene,
                                   const RayIntersection & intersection,
                                   const PointLight & light )
{
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
        return RGBColor(0.0f, 0.0f, 0.0f);
    }

    // Not in shadow
    float cos_r_n = fabsf( dot( direction, intersection.normal ) ); 
    RGBColor color = light.band_power;
    color.scale(cos_r_n);
    color.scale(1.0f / to_light.magnitude_sq()); // distance falloff

    return color;
}

RGBColor Shader::samplePointLights( const Scene & scene,
                                    const RayIntersection & intersection )
{
    RGBColor totalColor;
    for( const PointLight & light : scene.point_lights ) {
        const RGBColor color = samplePointLight( scene, intersection, light );
        totalColor.accum( mult( color, intersection.material->diffuse(intersection) ) );
    }
    return totalColor;
}

RGBColor Shader::sampleAreaLight( const Scene & scene,
                                  const RayIntersection & intersection,
                                  RandomNumberGenerator & rng,
                                  const AreaLight & light )
{
    auto sample = light.sampleSurfaceTransformed( rng );
    auto to_light = subtract( sample.position, intersection.position );

    if( dot( to_light, intersection.normal ) <= 0.0 ) {
        return RGBColor(0.0f, 0.0, 0.0f);
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
            return RGBColor(0.0f, 0.0, 0.0f);
        }
    }

    // Not in shadow
    float cos_r_n = fabsf( dot( direction, intersection.normal ) ); 
    RGBColor color = light.material->emittance;
    color.scale( cos_r_n );
    color.scale( fabsf( dot( sample.normal, direction.negated() ) ) ); // account for projected area of the light
    // FIXME - Should there be a distance falloff? Things seem too bright for objects close to lights if we do
    //color.scale( 1.0f / dist_sq_to_light ); // distance falloff
    return color;
}

RGBColor Shader::sampleAreaLights( const Scene & scene,
                                   const RayIntersection & intersection,
                                   RandomNumberGenerator & rng )
{
    RGBColor totalColor;
    for( const auto & light : scene.area_lights ) {
        const RGBColor color = sampleAreaLight( scene, intersection, rng, *light );
        // TODO: use actual material parameters properly so we can get specular here, too
        totalColor.accum( mult( color, intersection.material->diffuse(intersection) ) );
    }
    return totalColor;
}

RGBColor Shader::sampleEnvironmentMap( const Scene & scene,
                                       const RayIntersection & intersection,
                                       RandomNumberGenerator & rng )
{
    if( !scene.env_map || !scene.env_map->canImportanceSample() )
        return RGBColor(0.0f, 0.0, 0.0f);

    auto & envmap = *scene.env_map;

    EnvironmentMap::ImportanceSample isamp;

    // Find a sample in the direction of the normal
    float s_dot_n = -1.0;
    const unsigned int max_tries = 100;
    for( unsigned int tries = 0; tries < max_tries && s_dot_n < 0.0f; tries++ ) {
        isamp = envmap.importanceSample( rng, intersection );
        s_dot_n = dot( isamp.ray.direction, intersection.normal);
    }

    // Shoot a ray toward the light to see if we are in shadow
    Ray shadow_ray = isamp.ray;
    RayIntersection shadow_isect;
    shadow_isect.min_distance = EPSILON;
    if( s_dot_n > 0.0f &&
        !scene.intersect( shadow_ray, shadow_isect ) ) {
        // Not in shadow
        auto sample = envmap.sample(isamp.ray);
        float cos_r_n = fabsf( dot( isamp.ray.direction, intersection.normal ) ); 
        auto color = sample.color;
        color.scale(1.0f / isamp.pdf);
        color.scale(cos_r_n);
        //color.scale(0.01f); // TEMP
        //color.scale(4.0f); // TEMP - brightening it to see better
        //color.scale(2.0f*M_PI); // FIXME: Is this right?
        //color.scale(M_PI); // FIXME: Is this right?
        return mult( color, intersection.material->diffuse(intersection) );
    }

    return RGBColor(0.0f, 0.0f, 0.0f);
}

