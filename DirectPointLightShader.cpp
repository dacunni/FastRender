
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

        if( inShadow( scene, intersection, toLight ) ) {
            continue;
        }

        float cos_r_n = fabsf( dot( direction, intersection.normal ) ); 
        RGBColor Li = light.band_power;
        Li.scale(1.0f / toLight.magnitude_sq()); // distance falloff

        // TODO: get from material properties
        // TODO: should kd be based on fresnel?
        float kd = 0.2f;
        float ks = 1.0f - kd;

        float bxdf = intersection.material->BxDF(intersection.normal, intersection.fromDirection(), toLight);

        RGBColor diffuse = intersection.material->diffuseColor.scaled(kd / M_PI);
        RGBColor specular = RGBColor(ks * bxdf);

        RGBColor Lo = (diffuse + specular) * Li * cos_r_n;

        totalRadiance.accum(Lo);
    }

    intersection.sample.color = totalRadiance;
}


