#ifndef FastRender_Vector_hpp
#define FastRender_Vector_hpp

#include <cmath>
#include <algorithm>
#include "GeometryUtils.h"

inline Vector4::Vector4()
    : x(0.0f), y(0.0f), z(0.0f), w(1.0f)
{

}

inline Vector4::Vector4( const Vector4 & a )
{
	x = a.x;
	y = a.y;
	z = a.z;
	w = a.w;
}

inline Vector4::Vector4( float xn, float yn, float zn, float wn )
{
	x = xn;
	y = yn;
	z = zn;
	w = wn;
}

inline float & Vector4::operator[]( int i )
{
    return data[i];
}

inline const float & Vector4::operator[]( int i ) const
{
    return data[i];
}

inline void Vector4::set( float xn, float yn, float zn, float wn )
{
	x = xn;
	y = yn;
	z = zn;
	w = wn;
}

inline float Vector4::magnitude_sq() const
{
	return sq(x) + sq(y) + sq(z);
}

inline float Vector4::magnitude() const
{
	return std::sqrt( sq(x) + sq(y) + sq(z) );
}

inline void Vector4::normalize()
{
	float mag = magnitude();
	
	if( mag != 0.0 ) {
		scale( *this, 1.0f / mag, *this );
	}
}

inline Vector4 Vector4::normalized() const
{
	float mag = magnitude();
	
	if( mag != 0.0 ) {
        float invmag = 1.0f / mag;
        return Vector4( x * invmag, y * invmag, z * invmag, w );
    }
    return *this;
}

inline void Vector4::negate()
{
    x = -x;
    y = -y;
    z = -z;
}

inline Vector4 Vector4::negated() const
{
    return Vector4( -x, -y, -z, w );
}

inline bool Vector4::isUnity() const
{
    float m = magnitude();
    return m > 0.99f && m < 1.01f;
}

inline bool Vector4::isDirection() const
{
    return w == 0.0f;
}

// TODO - Decide whether to normalize by 1/w before doing this
inline void dot( const Vector4 & a, const Vector4 & b, float & r )
{
	r = a.x * b.x + a.y * b.y + a.z * b.z;
}

// TODO - Decide whether to normalize by 1/w before doing this
inline float dot( const Vector4 & a, const Vector4 & b )
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float clampedDot( const Vector4 & a, const Vector4 & b )
{
    return std::max(dot(a, b), 0.0f);
}

// TODO - Decide whether to normalize by 1/w before doing this
inline void cross( const Vector4 & a, const Vector4 & b, Vector4 & r )
{
	r.x = a.y * b.z - a.z * b.y;
	r.y = a.z * b.x - a.x * b.z;
	r.z = a.x * b.y - a.y * b.x;
}

// TODO - Decide whether to normalize by 1/w before doing this
inline Vector4 cross( const Vector4 & a, const Vector4 & b )
{
    return Vector4( a.y * b.z - a.z * b.y,
                    a.z * b.x - a.x * b.z,
                    a.x * b.y - a.y * b.x );
}

inline void add( const Vector4 & a, const Vector4 & b, Vector4 & r )
{
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;
	r.w = 1.0;
}

inline Vector4 add( const Vector4 & a, const Vector4 & b )
{
    return Vector4( a.x + b.x,
                    a.y + b.y,
                    a.z + b.z,
                    1.0 );
}

inline void subtract( const Vector4 & a, const Vector4 & b, Vector4 & r )
{
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	r.z = a.z - b.z;
	r.w = 1.0;
}

inline Vector4 subtract( const Vector4 & a, const Vector4 & b )
{
    return Vector4( a.x - b.x,
                    a.y - b.y,
                    a.z - b.z,
                    1.0 );
}

inline void scale( const Vector4 & a, float s, Vector4 & r )
{
	r.x = a.x * s;
	r.y = a.y * s;
	r.z = a.z * s;
	r.w = a.w;
}

inline Vector4 scale( const Vector4 & a, float s )
{
    return Vector4( a.x * s,
	                a.y * s,
	                a.z * s,
	                a.w );
}

inline void perspective_scale( const Vector4 & a, Vector4 & r )
{
	if( a.w != 0.0 ) {
		r.x = a.x / a.w;
		r.y = a.y / a.w;
		r.z = a.z / a.w;
		r.w = 1.0;
	}
}

inline Vector4 blend( const Vector4 & a, float s, const Vector4 & b, float t )
{
    return add( scale( a, s ), scale( b, t ) );
}

inline Vector4 perspective_scale( const Vector4 & a )
{
	if( a.w != 0.0 ) {
        return Vector4( a.x / a.w,
                        a.y / a.w,
                        a.z / a.w,
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

    r = blend( a.negated(), eta, n, eta * c1 - std::sqrt(c2sq) );
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
