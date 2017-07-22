
#include <algorithm>
#include "SignedDistanceFunction.h"
#include "Types.h"

// Distance from the level set considered a hit
const float hitDistance = 0.01;
// Distance to step when computing partial derivatives for estimating the normal
const float normalDelta = 0.001;
// Maximum number of steps to make tracing a SDF
const unsigned int maxSteps = 15;

bool SignedDistanceFunction::intersectsAny( const Ray & ray, float min_distance ) const
{ 
    // throw-away variables
    Vector4 position;
    float signedDistance, distance;
    return walkRay( ray, min_distance, signedDistance, distance, position );
}

bool SignedDistanceFunction::intersect( const Ray & ray, RayIntersection & intersection ) const
{
    Vector4 position;
    float signedDistance, distance;

    if( walkRay( ray, intersection.min_distance, signedDistance, distance, position ) ) {
        populateIntersection( ray, signedDistance, distance, position, intersection );
        //intersection.print(); // TEMP
        return true;
    }

    return false;
}

// Walk a ray until we get a hit
inline bool SignedDistanceFunction::walkRay( const Ray & ray,
                                             float tInit,
                                             float & signedDistance,
                                             float & distance,
                                             Vector4 & position ) const
{
    // Initial state
    float t = tInit;
    position = add( scale( ray.direction, t ), ray.origin );
    signedDistance = valueFunction( position );
    distance = fabsf( signedDistance );

    // Get away from the surface that we won't immediately think we've hit it.
    // This should help with refractive surfaces.
    if( distance < hitDistance ) {
        t += hitDistance;
        position = add( scale( ray.direction, t ), ray.origin );
        signedDistance = valueFunction( position );
        distance = fabsf( signedDistance );
    }

    //printf("** init   t %f sd %f\n", t, signedDistance);

    // Step until we find a spot where we are within the hit distance threshold.
    // If we are in a positive region, we should converge fast, as we expect
    // to get closer on every step. If we are in a negative region, we may have to
    // walk away from a surface before converging on the next.
    for( unsigned int step = 0; step < maxSteps; step++ ) {
        t += distance;
        position = add( scale( ray.direction, t ), ray.origin );
        signedDistance = valueFunction( position );
        distance = fabsf( signedDistance );
        //printf("** step %u t %f sd %f d %f\n", step, t, signedDistance, distance);
        if( distance < hitDistance ) {
            //printf("HIT step %u sd %f\n", step, signedDistance);
            return true;
        }
    }
    return false;

}

inline void SignedDistanceFunction::populateIntersection( const Ray & ray,
                                                          float signedDistance,
                                                          float distance,
                                                          const Vector4 & position,
                                                          RayIntersection & intersection ) const
{
    float dvdx = valueFunction( add( position, Vector4( normalDelta, 0.0, 0.0 ) ) ) - signedDistance;
    float dvdy = valueFunction( add( position, Vector4( 0.0, normalDelta, 0.0 ) ) ) - signedDistance;
    float dvdz = valueFunction( add( position, Vector4( 0.0, 0.0, normalDelta ) ) ) - signedDistance;
    intersection.normal = Vector4( dvdx, dvdy, dvdz ).normalized();
    intersection.distance = distance;
    intersection.ray = ray;
    intersection.position = position;
    intersection.material = material;
    intersection.traceable = this;
}

SignedDistanceFunction::ValueFunctionType makeSDFSphere( const Vector4 & center, float radius ) {
    return [center, radius](const Vector4 & v) { return subtract( v, center ).magnitude() - radius; };
}

SignedDistanceFunction::ValueFunctionType makeSDFBox( const Vector4 & center, const Vector4 & dims ) {
    using std::min;
    using std::max;

    return [center, dims](const Vector4 & v) {
        Vector4 p = subtract( v, center );
        Vector4 d = subtract( Vector4( fabs(p.x), fabs(p.y), fabs(p.z) ), scale( dims, 0.5 ) );
        Vector4 dc( max(d.x, 0.0f), max(d.y, 0.0f), max(d.z, 0.0f) );
        return (float) min(max(d.x, max(d.y, d.z)), 0.0f) + dc.magnitude();
    };
}

SignedDistanceFunction::ValueFunctionType makeSDFTorus( float mainRadius, float tubeRadius )
{
    return [mainRadius, tubeRadius](const Vector4 & v) {
        float2 q( sqrtf( sq(v.x) + sq(v.z) ) - mainRadius, v.y );
        return sqrtf( sq(q.x) + sq(q.y) ) - tubeRadius;
    };
}


SignedDistanceFunction::ValueFunctionType makeSDFCylinder( float radius )
{
    return [radius](const Vector4 & v) {
        return sqrtf( sq(v.x) + sq(v.z) ) - radius;
    };
}


