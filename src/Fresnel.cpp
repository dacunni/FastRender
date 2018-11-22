#include <cmath>

#include "Vector.h"
#include "GeometryUtils.h"

namespace Fresnel {

// Schlick approximation for
float Schlick( float F0, float cos_t )
{
    return F0 + (1.0f - F0) * std::pow(1.0f - cos_t, 5.0f);
}

namespace Dialectric {

static float SqrtParallel( float cos_i, float cos_t, float n_i, float n_t )
{
    float denom_pa = (n_i * cos_i + n_t * cos_t);
    if( denom_pa == 0.0f ) {
        denom_pa = 1.0f; // FIXME?
    }
    return (n_i * cos_i - n_t * cos_t) / denom_pa;
}

float Parallel( float cos_i, float cos_t, float n_i, float n_t )
{
    return sq(SqrtParallel(cos_i, cos_t, n_i, n_t));
}

static float SqrtPerpendicular( float cos_i, float cos_t, float n_i, float n_t )
{
    float denom_pe = (n_i * cos_t + n_t * cos_i);
    if( denom_pe == 0.0f ) {
        denom_pe = 1.0f; // FIXME?
    }
    return (n_i * cos_t - n_t * cos_i) / denom_pe;
}

float Perpendicular( float cos_i, float cos_t, float n_i, float n_t )
{
    return sq(SqrtPerpendicular(cos_i, cos_t, n_i, n_t));
}

// Fresnel formula for reflectance of a dialectric (non-conductive) material
// (eg: glass)
float Unpolarized( float cos_i, float cos_t, float n_i, float n_t )
{
    // for unpolarized light, we use the average of parallel and perpendicular terms
    return 0.5 * (Parallel(cos_i, cos_t, n_i, n_t) +
                  Perpendicular(cos_i, cos_t, n_i, n_t));
}

// Fresnel formula for reflectance of a dialectric (non-conductive) material
// taking Snell's law into account to derive cos_t
float Snell( float cos_i, float n_i, float n_t )
{
    float angle_i = acos(cos_i);
    float angle_t = snellsLawAngle(n_i, angle_i, n_t);
    return Unpolarized(cos_i, cos(angle_t), n_i, n_t);
}

float ParallelSnell( float cos_i, float n_i, float n_t )
{
    float angle_i = acos(cos_i);
    float angle_t = snellsLawAngle(n_i, angle_i, n_t);
    return Parallel(cos_i, cos(angle_t), n_i, n_t);
}

float PerpendicularSnell( float cos_i, float n_i, float n_t )
{
    float angle_i = acos(cos_i);
    float angle_t = snellsLawAngle(n_i, angle_i, n_t);
    return Perpendicular(cos_i, cos(angle_t), n_i, n_t);
}

float AtNormal( float n_i, float n_t, float k_t )
{
    float num   = sq(n_t - n_i) + sq(k_t);
    float denom = sq(n_t + n_i) + sq(k_t);
    return num / denom;
}

}; // Dialectric

namespace Conductor {

// Fresnel formula for reflectance of a conductive material (eg: metals)
// (approximate)
float Unpolarized( float cos_i, float n, float k )
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

float AtNormal( float n_i, float n_t )
{
    float num   = sq(n_t - n_i);
    float denom = sq(n_t + n_i);
    return num / denom;
}

}; // Conductor

}; // Fresnel

