//
//  Vector.hpp
//  FastRender
//
//  Created by David Cunningham on 8/3/13.
//
//

#ifndef FastRender_Vector_hpp
#define FastRender_Vector_hpp

#include <cmath>
#include "GeometryUtils.h"

inline Vector4::Vector4()
    : x(0.0f), y(0.0f), z(0.0f), w(1.0f)
{

}

inline Vector4::Vector4( const Vector4 & a )
{
	data[0] = a.data[0];
	data[1] = a.data[1];
	data[2] = a.data[2];
	data[3] = a.data[3];
}

inline Vector4::Vector4( float xn, float yn, float zn, float wn )
{
	data[0] = xn;
	data[1] = yn;
	data[2] = zn;
	data[3] = wn;
}

inline void Vector4::set( float xn, float yn, float zn, float wn )
{
	data[0] = xn;
	data[1] = yn;
	data[2] = zn;
	data[3] = wn;
}

// TODO - Decide whether to normalize by 1/w before doing this
inline void dot( const Vector4 & a, const Vector4 & b, float & r )
{
	r = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

// TODO - Decide whether to normalize by 1/w before doing this
inline float dot( const Vector4 & a, const Vector4 & b )
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

// TODO - Decide whether to normalize by 1/w before doing this
inline void cross( const Vector4 & a, const Vector4 & b, Vector4 & r )
{
	r[0] = a[1] * b[2] - a[2] * b[1];
	r[1] = a[2] * b[0] - a[0] * b[2];
	r[2] = a[0] * b[1] - a[1] * b[0];
}

// TODO - Decide whether to normalize by 1/w before doing this
inline Vector4 cross( const Vector4 & a, const Vector4 & b )
{
    return Vector4( a[1] * b[2] - a[2] * b[1],
                    a[2] * b[0] - a[0] * b[2],
                    a[0] * b[1] - a[1] * b[0] );
}

inline void add( const Vector4 & a, const Vector4 & b, Vector4 & r )
{
	r[0] = a[0] + b[0];
	r[1] = a[1] + b[1];
	r[2] = a[2] + b[2];
	r[3] = 1.0;
}

inline Vector4 add( const Vector4 & a, const Vector4 & b )
{
    return Vector4( a[0] + b[0],
                    a[1] + b[1],
                    a[2] + b[2],
                    1.0 );
}

inline void subtract( const Vector4 & a, const Vector4 & b, Vector4 & r )
{
	r[0] = a[0] - b[0];
	r[1] = a[1] - b[1];
	r[2] = a[2] - b[2];
	r[3] = 1.0;
}

inline Vector4 subtract( const Vector4 & a, const Vector4 & b )
{
    return Vector4( a[0] - b[0],
                    a[1] - b[1],
                    a[2] - b[2],
                    1.0 );
}

inline void scale( const Vector4 & a, float s, Vector4 & r )
{
	r[0] = a[0] * s;
	r[1] = a[1] * s;
	r[2] = a[2] * s;
	r[3] = a[3];
}

inline Vector4 scale( const Vector4 & a, float s )
{
    return Vector4( a[0] * s,
	                a[1] * s,
	                a[2] * s,
	                a[3] );
}

inline void perspective_scale( const Vector4 & a, Vector4 & r )
{
	if( a[3] != 0.0 ) {
		r[0] = a[0] / a[3];
		r[1] = a[1] / a[3];
		r[2] = a[2] / a[3];
		r[3] = 1.0;
	}
}

inline Vector4 blend( const Vector4 & a, float s, const Vector4 & b, float t )
{
    return add( scale( a, s ), scale( b, t ) );
}

inline Vector4 perspective_scale( const Vector4 & a )
{
	if( a[3] != 0.0 ) {
        return Vector4( a[0] / a[3],
                        a[1] / a[3],
                        a[2] / a[3],
                        1.0 );
	}
    else {
        return a;
    }
}

// Given vector A pointing away from surface and normal N, computes the mirror direction R as
//
//  R = 2 * dot(A,N) * N - A
//
inline void mirror( const Vector4 & a, const Vector4 & n, Vector4 & r )
{
    float twoAdotN = 2.0f * dot( a, n );
    Vector4 NtwoAdotN = scale( n, twoAdotN );
    r = subtract( NtwoAdotN, a );
    r.makeDirection();
}

inline Vector4 mirror( const Vector4 & a, const Vector4 & n )
{
    Vector4 r;
    mirror( a, n, r );
    return r;
}

// Given vector A pointing away from surface and normal N, as well as the
// indices of refraction of the original material (n1) and incident material
// (n2), computes the refraction direction R.
//
// Assuming A and N are proper directions (length = 1)
//
// Reference:
//   http://steve.hollasch.net/cgindex/render/refraction.txt
//
inline void refract( const Vector4 & a, const Vector4 & n, float n1, float n2,
                     Vector4 & r )
{
    float eta = n1 / n2;
    float c1 = dot( a, n );                          // cos(theta1)
    float c2sq = 1.0 - eta * eta * (1.0 - c1 * c1);  // cos(theta2)

    if( c2sq < 0.0 ) { // Total internal reflection
        r.x = r.y = r.z = r.w = 0.0;
        return;
    }

    r = blend( a.negated(), eta, n, eta * c1 - sqrt(c2sq) );
    r.normalize();
    r.makeDirection();
}

inline Vector4 refract( const Vector4 & a, const Vector4 & n, float n1, float n2 )
{
    Vector4 r;
    refract( a, n, n1, n2, r );
    return r;
}


// Linearly interpolate two vectors using parameter alpha, such that alpha=0 gives a and
// alpha=1 gives b
inline void interp( const Vector4 & a, const Vector4 & b, const float alpha, Vector4 & r)
{
    float oma = 1.0f - alpha;
    r.x = oma * a.x + alpha * b.x;
    r.y = oma * a.y + alpha * b.y;
    r.z = oma * a.z + alpha * b.z;
    r.w = oma * a.w + alpha * b.w;
}

inline Vector4 interp( const Vector4 & a, const Vector4 & b, const float alpha )
{
    float oma = 1.0f - alpha;
    return Vector4( oma * a.x + alpha * b.x,
                    oma * a.y + alpha * b.y,
                    oma * a.z + alpha * b.z,
                    oma * a.w + alpha * b.w );
}

#endif
