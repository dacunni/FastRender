
#include <cassert>
#include <iostream>
#include "Material.h"
#include "Scene.h"
#include "Ray.h"
#include "DirectEnvironmentMapShader.h"
#include "RandomNumberGenerator.h"
#include "AxisAlignedSlab.h"
#include "EnvironmentMap.h"

void DirectEnvironmentMapShader::shade( Scene & scene, RandomNumberGenerator & rng, RayIntersection & intersection )
{
    intersection.sample.color = sampleEnvironmentMap(scene, intersection, rng);
}


