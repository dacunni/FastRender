
#include "Lights.h"
#include "Ray.h"
#include "Material.h"
#include "RandomNumberGenerator.h"

AreaLight::AreaLight( const RGBColor & emittance ) { material = std::make_shared<DiffuseEmitterMaterial>(emittance); }
AreaLight::~AreaLight() {}

// FIXME: This won't work for lights in containers that have transforms
LightSample AreaLight::sampleSurfaceTransformed( RandomNumberGenerator & rng ) const
{
    auto sample = sampleSurface( rng );
    sample.normal.assertIsDirection();

    if( transform ) {
        sample.position = mult( transform->fwd, sample.position );
        sample.normal = mult( transform->fwd, sample.normal );
    }

    return sample;
}

CircleAreaLight::CircleAreaLight() : AreaLight(RGBColor(1, 1, 1)), radius(1.0) {}
CircleAreaLight::CircleAreaLight( float r, const RGBColor & emittance ) : AreaLight(emittance), radius(r) {}
CircleAreaLight::~CircleAreaLight() {}

bool CircleAreaLight::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    // Make sure the ray isn't parallel to the XZ plane
    if( ray.direction.y == 0.0f ) {
        return false;
    }

    // Calculate the intersection of the ray with the XZ plane
    // o.y + d.y * t = 0  =>  t = -o.y / d.y
    float t = -ray.origin.y / ray.direction.y;

    // Make sure we are at least min_distance out from the ray origin, and that t > 0
    if( t < intersection.min_distance ) {
        return false;
    }

    auto pos = add( ray.origin, scale( ray.direction, t ) );

    // Check that the intersection point is within the radius
    if( sq(pos.x) + sq(pos.z) > radius * radius ) {
        return false;
    }

    // Record ray intersection
    intersection.ray = ray;
    intersection.distance = t;
    intersection.position = pos;
    intersection.normal = Vector4( 0.0f, ray.direction.y >= 0.0f ? -1.0f : 1.0f, 0.0f );
    intersection.normal.makeDirection();
    intersection.material = material;
    intersection.traceable = this;

    return true;
}

LightSample CircleAreaLight::sampleSurface( RandomNumberGenerator & rng ) const
{
    LightSample sample = {
        .position = Vector4(0, 0, 0),
        .normal = Vector4(0, -1, 0, 0)
    };
    
    rng.uniformCircle( radius, sample.position.x, sample.position.z );

    return sample;
}

float CircleAreaLight::area() const
{
    return M_PI * radius * radius;
}

RectangleAreaLight::RectangleAreaLight() : AreaLight(RGBColor(1, 1, 1)), xdim(1.0), zdim(1.0) {}
RectangleAreaLight::RectangleAreaLight( float xd, float zd, const RGBColor & emittance ) : AreaLight(emittance), xdim(xd), zdim(zd) {}
RectangleAreaLight::~RectangleAreaLight() {}

bool RectangleAreaLight::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    // Make sure the ray isn't parallel to the XZ plane
    if( ray.direction.y == 0.0f ) {
        return false;
    }

    // Calculate the intersection of the ray with the XZ plane
    // o.y + d.y * t = 0  =>  t = -o.y / d.y
    float t = -ray.origin.y / ray.direction.y;

    // Make sure we are at least min_distance out from the ray origin, and that t > 0
    if( t < intersection.min_distance ) {
        return false;
    }

    auto pos = add( ray.origin, scale( ray.direction, t ) );

    // Check that the intersection point is within the rectangle
    if( fabsf(pos.x) > xdim * 0.5f || fabsf(pos.z) > zdim * 0.5f ) {
        return false;
    }

    // Record ray intersection
    intersection.ray = ray;
    intersection.distance = t;
    intersection.position = pos;
    intersection.normal = Vector4( 0.0f, ray.direction.y >= 0.0f ? -1.0f : 1.0f, 0.0f );
    intersection.normal.makeDirection();
    intersection.material = material;
    intersection.traceable = this;

    return true;
}

LightSample RectangleAreaLight::sampleSurface( RandomNumberGenerator & rng ) const
{
    LightSample sample = {
        .position = Vector4(0, 0, 0),
        .normal = Vector4(0, -1, 0, 0)
    };
    
    sample.position.x = rng.uniformRange( -xdim * 0.5, xdim * 0.5 );
    sample.position.y = rng.uniformRange( -zdim * 0.5, zdim * 0.5 );

    return sample;
}

float RectangleAreaLight::area() const
{
    return xdim * zdim;
}

