#ifndef _RAY_H_
#define _RAY_H_

#include <memory>
#include <string>
#include <float.h>

#include "Vector.h"
#include "Color.h"

class Scene;
class Traceable;
class Material;
extern std::shared_ptr<Material> DEFAULT_MATERIAL;

//const float EPSILON = 0.01;
const float EPSILON = 0.001;

class Ray {
public:
    Ray() : depth(1) {}
    Ray( const Vector4 & o, const Vector4 & d ) : origin(o), direction(d), depth(1) {}
    ~Ray() = default;

    std::string toString() const;

    Vector4 origin;
    Vector4 direction;
    unsigned char depth;
    float index_of_refraction = 1.0f;  // index of the material we're in
};

class RGBRadianceSample {
public:
    RGBRadianceSample() : color(0.0f, 0.0f, 0.0f) {}
    RGBRadianceSample(const RGBColor & c) : color(c) {}
    ~RGBRadianceSample() = default;

    RGBColor color;
};

class RayIntersection {
public:
    RayIntersection()
        : min_distance(0.0f), distance(FLT_MAX),
          material(DEFAULT_MATERIAL) // So we can assume there is always a material in tracing logic
    {}
    ~RayIntersection() = default;

    Vector4 fromDirection() const;

    void print();

    Ray ray;
    Vector4 position;
    Vector4 normal;
    RGBRadianceSample sample;
    std::shared_ptr<Material> material;
    float u, v;
    const Traceable * traceable; // For debugging
    float distance;
    float min_distance;
};

class Sample {
public:
    float value;
};

void trace( Ray & ray, Scene & scene, RayIntersection & intersection, Sample & sample );
void shade( Scene & scene, RayIntersection & intersection, Sample & sample );

#endif
