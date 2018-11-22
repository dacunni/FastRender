#ifndef _FRESNEL_H_
#define _FRESNEL_H_

namespace Fresnel {

float Schlick( float F0, float cos_t );

namespace Dialectric {

float Unpolarized( float cos_i, float cos_t, float n_i, float n_t );
float Parallel( float cos_i, float cos_t, float n_i, float n_t );
float Perpendicular( float cos_i, float cos_t, float n_i, float n_t );
float Snell( float cos_i, float n_i, float n_t );
float ParallelSnell( float cos_i, float n_i, float n_t );
float PerpendicularSnell( float cos_i, float n_i, float n_t );
float AtNormal( float n_i, float n_t, float k_t );

}; // Dialectric

namespace Conductor {

float Unpolarized( float cos_i, float n, float k );
float AtNormal( float n_i, float n_t );

}; // Conductor

}; // Fresnel

#endif
