
#include "Lights.h"
#include "Ray.h"
#include "Material.h"

AreaLight::AreaLight( const RGBColor & emittance ) { material = std::make_shared<DiffuseEmitterMaterial>(emittance); }
AreaLight::~AreaLight() {}

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

