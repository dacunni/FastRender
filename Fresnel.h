#ifndef _FRESNEL_H_
#define _FRESNEL_H_

namespace Fresnel {

float Dialectric( float cos_i, float cos_t, float n_i, float n_t );
float DialectricParallel( float cos_i, float cos_t, float n_i, float n_t );
float DialectricPerpendicular( float cos_i, float cos_t, float n_i, float n_t );
float DialectricSnell( float cos_i, float n_i, float n_t );
float DialectricParallelSnell( float cos_i, float n_i, float n_t );
float DialectricPerpendicularSnell( float cos_i, float n_i, float n_t );
float DialectricSchlick( float F0, float cos_t );

float Conductor( float cos_i, float n, float k );

}; // Fresnel

#endif
