#include <cassert>
#include <iostream>
#include "Material.h"
#include "Scene.h"
#include "Ray.h"
#include "DirectLightingShaders.h"
#include "RandomNumberGenerator.h"
#include "AxisAlignedSlab.h"
#include "EnvironmentMap.h"

void DirectPointLightShader::shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection )
{
    intersection.sample.color = samplePointLights(scene, intersection);
}

void DirectAreaLightShader::shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection )
{
    intersection.sample.color = sampleAreaLights(scene, intersection, rng);
}

void DirectEnvironmentMapShader::shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection )
{
    intersection.sample.color = sampleEnvironmentMap(scene, intersection, rng);
}


