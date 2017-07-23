/*
 *  GeometryUtils.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 12/18/2015.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include <math.h>

#include "Vector.h"
#include "GeometryUtils.h"


// Calculate the area of a spherical cap given by the half angle
// subtended by the sphere about its center axis
//
//      http://mathworld.wolfram.com/SphericalCap.html
//
// TODO[DAC]: This seems to work for angles > PI / 2, but need to check to be sure
//
float sphericalCapSurfaceArea( float half_angle, float radius )
{
    float cap_height = radius * ( 1.0f - cos( half_angle ) );
    float contact_angle = M_PI * 0.5f - half_angle;
    float base_radius = sin( half_angle );
    float cap_area = M_PI * ( sq( base_radius ) + sq( cap_height ) );

    // printf( "cap_height    = %f\n", cap_height );
    // printf( "contact_angle = %f\n", contact_angle );
    // printf( "base_radius   = %f\n", base_radius );
    // printf( "cap_area      = %f\n", cap_area  );

    return cap_area;
}

// Calculate the area of a triangle given its vertices
float triangleArea( const Vector4 & A, const Vector4 & B, const Vector4 & C )
{
    // Area of the parallelogram with AB and AC as sides
    auto AB = subtract( B, A );
    auto AC = subtract( C, A );
    auto ABxAC = cross( AB, AC );
    float paraArea = ABxAC.magnitude();
    // The triangle is half of this parallegram
    return paraArea * 0.5;
}

// Finds the barycentric coordinate of a point in a triangle. Assumes the point
// is inside the triangle (in the same plane).
BarycentricCoordinate barycentricForPointInTriangle( const Vector4 & P,
                                                     const Vector4 & tA, const Vector4 & tB, const Vector4 & tC )
{
    float tArea = triangleArea( tA, tB, tC );

    if( tArea <= 0.0f ) { // degenerate triangle
        return BarycentricCoordinate( 0.0, 0.0 );
    }

    return BarycentricCoordinate( triangleArea( tB, tC, P ) / tArea,
                                  triangleArea( tC, tA, P ) / tArea );
}

Vector4 interpolate( const Vector4 & v0,
                     const Vector4 & v1,
                     const Vector4 & v2,
                     const BarycentricCoordinate & bary )
{
    return add( add( scale( v0, bary.u ),
                     scale( v1, bary.v ) ),
                scale( v2, bary.w ) ).normalized();
}

// -----------------------------------------------------------------------------
// Fresnel Equations
//

static float fresnelDialectricSqrtParallel( float cos_i, float cos_t, float n_i, float n_t )
{
    float denom_pa = (n_i * cos_i + n_t * cos_t);
    if( denom_pa == 0.0f ) {
        denom_pa = 1.0f; // FIXME?
    }
    return (n_i * cos_i - n_t * cos_t) / denom_pa;
}

float fresnelDialectricParallel( float cos_i, float cos_t, float n_i, float n_t )
{
    return sq(fresnelDialectricSqrtParallel(cos_i, cos_t, n_i, n_t));
}

static float fresnelDialectricSqrtPerpendicular( float cos_i, float cos_t, float n_i, float n_t )
{
    float denom_pe = (n_i * cos_t + n_t * cos_i);
    if( denom_pe == 0.0f ) {
        denom_pe = 1.0f; // FIXME?
    }
    return (n_i * cos_t - n_t * cos_i) / denom_pe;
}

float fresnelDialectricPerpendicular( float cos_i, float cos_t, float n_i, float n_t )
{
    return sq(fresnelDialectricSqrtPerpendicular(cos_i, cos_t, n_i, n_t));
}

// Fresnel formula for reflectance of a dialectric (non-conductive) material
// (eg: glass)
float fresnelDialectric( float cos_i, float cos_t, float n_i, float n_t )
{
    // for unpolarized light, we use the average of parallel and perpendicular terms
    return 0.5 * (fresnelDialectricParallel(cos_i, cos_t, n_i, n_t) +
                  fresnelDialectricPerpendicular(cos_i, cos_t, n_i, n_t));
}

// Fresnel formula for reflectance of a dialectric (non-conductive) material
// taking Snell's law into account to derive cos_t
float fresnelDialectricSnell( float cos_i, float n_i, float n_t )
{
    float angle_i = acos(cos_i);
    float angle_t = snellsLawAngle(n_i, angle_i, n_t);
    return fresnelDialectric(cos_i, cos(angle_t), n_i, n_t);
}

float fresnelDialectricParallelSnell( float cos_i, float n_i, float n_t )
{
    float angle_i = acos(cos_i);
    float angle_t = snellsLawAngle(n_i, angle_i, n_t);
    return fresnelDialectricParallel(cos_i, cos(angle_t), n_i, n_t);
}

float fresnelDialectricPerpendicularSnell( float cos_i, float n_i, float n_t )
{
    float angle_i = acos(cos_i);
    float angle_t = snellsLawAngle(n_i, angle_i, n_t);
    return fresnelDialectricPerpendicular(cos_i, cos(angle_t), n_i, n_t);
}

// Fresnel formula for reflectance of a conductive material (eg: metals)
// (approximate)
float fresnelConductor( float cos_i, float n, float k )
{
    const float nspks = sq(n) + sq(k);
    const float cs = sq(cos_i);
    const float nspkscs = nspks * cs;
    float twonc = 2.0f * n * cos_i;
    float R_pa2 = (nspkscs - twonc + 1.0f) /
                  (nspkscs + twonc + 1.0f);
    float R_pe2 = (nspks - twonc + cs) /
                  (nspks + twonc + cs);
    return (R_pa2 + R_pe2) * 0.5f;
}

// -----------------------------------------------------------------------------
// Snell's Law
//
//   n1 * sin(a1) = n2 * sin(a2)
//
// Angle formulation
float snellsLawAngle( float n_i, float angle_i, float n_o )
{
    return asinf( n_i / n_o * sinf(angle_i) );
}

// Sine formulation
float snellsLawSine( float n_i, float sin_i, float n_o )
{
    return n_i / n_o * sin_i;
}




