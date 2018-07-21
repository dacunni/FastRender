
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
    intersection.sample.color = samplePointLights(scene, intersection);
}


