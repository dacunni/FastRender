/*
 *  GeometryUtils.h
 *  FastRender
 *
 *  Created by David Cunningham on 12/18/2015
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _GEOMETRY_UTILS_H_
#define _GEOMETRY_UTILS_H_


// Calculate the area of a spherical cap given by the half angle
// subtended by the sphere about its center axis
float sphericalCapSurfaceArea( float half_angle, float radius = 1.0f );

// Calculate the area of a triangle given its vertices
float triangleArea( const Vector4 & A, const Vector4 & B, const Vector4 & C );

// Barycentric coordinates using the convension that u + v + w = 1
struct BarycentricCoordinate
{
    BarycentricCoordinate( float _u, float _v ) : u(_u), v(_v), w(1.0f - _u - _v) {}
    BarycentricCoordinate( float _u, float _v, float _w ) : u(_u), v(_v), w(_w) {}
    ~BarycentricCoordinate() {}
    float u, v, w;
};

// Finds the barycentric coordinate of a point in a triangle. Assumes the point
// is inside the triangle (in the same plane).
BarycentricCoordinate barycentricForPointInTriangle( const Vector4 & P,
                                                     const Vector4 & tA, const Vector4 & tB, const Vector4 & tC );

// -----------------------------------------------------------------------------
// Indices of refraction
//   (at 589 um, source: https://en.wikipedia.org/wiki/Refractive_index)

#define N_VACUUM            1.0
#define N_AIR               1.0001
#define N_WATER             1.333
#define N_ETHANOL           1.36
#define N_OLIVE_OIL         1.47
#define N_ICE               1.31
#define N_PLEXIGLAS         1.49
#define N_CROWN_GLASS       1.52
#define N_FLINT_GLASS       1.62
#define N_CUBIC_ZIRCONIA    2.15
#define N_DIAMOND           2.42
#define N_MOISSANITE        2.65
#define N_ZINC_SELENIDE     2.67

// -----------------------------------------------------------------------------
// Fresnel Equations
//
float fresnelDialectric( float cos_i, float cos_t, float n_i, float n_t );
float fresnelConductor( float cos_i, float cos_t, float n_i, float n_t );

// -----------------------------------------------------------------------------
// Snell's Law
//
// Angle formulation
float snellsLawAngle( float n_i, float angle_i, float n_o );
// Sine formulation
float snellsLawSine( float n_i, float sin_i, float n_o );


#endif
