
#include <cassert>
#include <iostream>
#include "Material.h"
#include "Scene.h"
#include "Ray.h"
#include "DirectPointLightShader.h"
#include "RandomNumberGenerator.h"
#include "AxisAlignedSlab.h"
#include "EnvironmentMap.h"

void DirectPointLightShader::shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection )
{
    RGBColor totalRadiance;

    for( const PointLight & light : scene.point_lights ) {
        Vector4 toLight = subtract( light.position, intersection.position );
        Vector4 direction = toLight.normalized();
        direction.makeDirection();

        RGBColor Li = samplePointLight( scene, intersection, light );
        totalRadiance += reflectedRadiance(intersection, Li, direction);
    }

    intersection.sample.color = totalRadiance;
}


