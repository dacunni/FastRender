#include "Shader.h"
#include "Lights.h"
#include "EnvironmentMap.h"
#include "Scene.h"
#include "Vector.h"

bool Shader::inShadow( const Scene & scene,
                       const RayIntersection & intersection,
                       const Vector4 & toLight )
{
    float dist_sq_to_light = toLight.magnitude_sq();
    Vector4 direction = toLight.normalized();
    direction.makeDirection();

    Ray shadow_ray( intersection.position, direction );
    RayIntersection shadow_isect;
    shadow_isect.min_distance = EPSILON;
    if( scene.intersect( shadow_ray, shadow_isect ) ) {
        float dist_sq_to_isect = sq(shadow_isect.distance + shadow_isect.min_distance);
        if( dist_sq_to_isect < dist_sq_to_light ) {
            return true;
        }
    }
    return false;
}

RGBColor Shader::samplePointLight( const Scene & scene,
                                   const RayIntersection & intersection,
                                   const PointLight & light )
{
    Vector4 to_light = subtract( light.position, intersection.position );
    float dist_sq_to_light = to_light.magnitude_sq();
    Vector4 direction = to_light.normalized();
    direction.makeDirection();

    if( inShadow( scene, intersection, to_light ) ) {
        return RGBColor(0.0f, 0.0f, 0.0f);
    }

    float cos_r_n = clampedDot( direction, intersection.normal ); 
    RGBColor color = light.band_power;
    color.scale(cos_r_n);
    color.scale(1.0f / to_light.magnitude_sq()); // distance falloff

    return color;
}

RGBColor Shader::samplePointLights( const Scene & scene,
                                    const RayIntersection & intersection )
{
    RGBColor totalRadiance;

    for( const PointLight & light : scene.point_lights ) {
        Vector4 toLight = subtract( light.position, intersection.position );
        Vector4 direction = toLight.normalized();
        direction.makeDirection();

        RGBColor Li = samplePointLight( scene, intersection, light );
        totalRadiance += reflectedRadiance(intersection, Li, direction);
    }

    return totalRadiance;
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

    if( inShadow( scene, intersection, to_light ) ) {
        return RGBColor(0.0f, 0.0f, 0.0f);
    }

    RGBColor color = light.material->emittance;
    color.scale( clampedDot( sample.normal, direction.negated() ) ); // account for projected area of the light
    // FIXME - Should there be a distance falloff? Things seem too bright for objects close to lights if we do
    //color.scale( 1.0f / dist_sq_to_light ); // distance falloff
    return color;
}

RGBColor Shader::sampleAreaLights( const Scene & scene,
                                   const RayIntersection & intersection,
                                   RandomNumberGenerator & rng )
{
    RGBColor totalRadiance;
    for( const auto & light : scene.area_lights ) {
        auto sample = light->sampleSurfaceTransformed( rng );
        auto toLight = subtract( sample.position, intersection.position );
        auto direction = toLight.normalized();
        direction.makeDirection();

        RGBColor Li = sampleAreaLight( scene, intersection, rng, *light ); 
        totalRadiance += reflectedRadiance(intersection, Li, direction);
    }

    return totalRadiance;
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
        isamp = envmap.importanceSample(rng, intersection);
        s_dot_n = dot(isamp.ray.direction, intersection.normal);
    }

    // Shoot a ray to see if we intersect the scene. If we don't, then sample the environment map
    Ray shadow_ray = isamp.ray;
    RayIntersection shadow_isect;
    shadow_isect.min_distance = EPSILON;
    if( s_dot_n >= 0.0f && !scene.intersect( shadow_ray, shadow_isect ) ) {
        // Not in shadow
        auto sample = envmap.sample(isamp.ray);
        RGBColor Li = sample.color;
        RGBColor Lo = reflectedRadiance(intersection, Li, isamp.ray.direction);
        Lo.scale(1.0f / isamp.pdf);
        Lo.scale(M_PI); // TEMP
        return Lo;
    }

    return RGBColor(0.0f, 0.0f, 0.0f);
}

RGBColor Shader::reflectedRadiance( const RayIntersection & intersection,
                                    const RGBColor & Li,
                                    const Vector4 & lightDirection )
{
    float cos_r_n = clampedDot(lightDirection, intersection.normal); 

    // TODO: should kd be based on fresnel?
    float ks = intersection.material->specularity();
    float kd = 1.0f - ks;
    float bxdf = intersection.material->BxDF(intersection.normal, intersection.fromDirection(), lightDirection);
    RGBColor diffuse = intersection.material->diffuse(intersection).scaled(kd / M_PI);
    RGBColor specular = RGBColor(ks * bxdf);
    RGBColor Lo = (diffuse + specular) * Li * cos_r_n;

    assert(Lo.isFinite());

    return Lo;
}

