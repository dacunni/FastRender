
#include <cassert>
#include <iostream>
#include "Material.h"
#include "Scene.h"
#include "Ray.h"
#include "DirectAreaLightShader.h"
#include "RandomNumberGenerator.h"
#include "AxisAlignedSlab.h"
#include "EnvironmentMap.h"

void DirectAreaLightShader::shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection )
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

    intersection.sample.color = totalRadiance;
}


