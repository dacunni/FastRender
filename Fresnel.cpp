#include <cmath>

#include "Vector.h"
#include "GeometryUtils.h"

namespace Fresnel {

static float DialectricSqrtParallel( float cos_i, float cos_t, float n_i, float n_t )
{
    float denom_pa = (n_i * cos_i + n_t * cos_t);
    if( denom_pa == 0.0f ) {
        denom_pa = 1.0f; // FIXME?
    }
    return (n_i * cos_i - n_t * cos_t) / denom_pa;
}

float DialectricParallel( float cos_i, float cos_t, float n_i, float n_t )
{
    return sq(DialectricSqrtParallel(cos_i, cos_t, n_i, n_t));
}

static float DialectricSqrtPerpendicular( float cos_i, float cos_t, float n_i, float n_t )
{
    float denom_pe = (n_i * cos_t + n_t * cos_i);
    if( denom_pe == 0.0f ) {
        denom_pe = 1.0f; // FIXME?
    }
    return (n_i * cos_t - n_t * cos_i) / denom_pe;
}

float DialectricPerpendicular( float cos_i, float cos_t, float n_i, float n_t )
{
    return sq(DialectricSqrtPerpendicular(cos_i, cos_t, n_i, n_t));
}

// Fresnel formula for reflectance of a dialectric (non-conductive) material
// (eg: glass)
float Dialectric( float cos_i, float cos_t, float n_i, float n_t )
{
    // for unpolarized light, we use the average of parallel and perpendicular terms
    return 0.5 * (DialectricParallel(cos_i, cos_t, n_i, n_t) +
                  DialectricPerpendicular(cos_i, cos_t, n_i, n_t));
}

// Fresnel formula for reflectance of a dialectric (non-conductive) material
// taking Snell's law into account to derive cos_t
float DialectricSnell( float cos_i, float n_i, float n_t )
{
    float angle_i = acos(cos_i);
    float angle_t = snellsLawAngle(n_i, angle_i, n_t);
    return Dialectric(cos_i, cos(angle_t), n_i, n_t);
}

float DialectricParallelSnell( float cos_i, float n_i, float n_t )
{
    float angle_i = acos(cos_i);
    float angle_t = snellsLawAngle(n_i, angle_i, n_t);
    return DialectricParallel(cos_i, cos(angle_t), n_i, n_t);
}

float DialectricPerpendicularSnell( float cos_i, float n_i, float n_t )
{
    float angle_i = acos(cos_i);
    float angle_t = snellsLawAngle(n_i, angle_i, n_t);
    return DialectricPerpendicular(cos_i, cos(angle_t), n_i, n_t);
}

// Schlick approximation for  of a dialectric
float DialectricSchlick( float F0, float cos_t )
{
    return F0 + (1.0f - F0) * std::pow(1.0f - cos_t, 5.0f);
}

// Fresnel formula for reflectance of a conductive material (eg: metals)
// (approximate)
float Conductor( float cos_i, float n, float k )
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

}; // Fresnel

