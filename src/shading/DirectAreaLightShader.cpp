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
    intersection.sample.color = sampleAreaLights(scene, intersection, rng);
}


