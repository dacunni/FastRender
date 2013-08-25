//
//  Vector.hpp
//  FastRender
//
//  Created by David Cunningham on 8/3/13.
//
//

#ifndef FastRender_Vector_hpp
#define FastRender_Vector_hpp

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

inline void add( const Vector4 & a, const Vector4 & b, Vector4 & r )
{
	r[0] = a[0] + b[0];
	r[1] = a[1] + b[1];
	r[2] = a[2] + b[2];
	r[3] = 1.0;
}

inline void subtract( const Vector4 & a, const Vector4 & b, Vector4 & r )
{
	r[0] = a[0] - b[0];
	r[1] = a[1] - b[1];
	r[2] = a[2] - b[2];
	r[3] = 1.0;
}

inline void scale( const Vector4 & a, float s, Vector4 & r )
{
	r[0] = a[0] * s;
	r[1] = a[1] * s;
	r[2] = a[2] * s;
	r[3] = a[3];
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

// Given vector A pointing away from surface and normal N, computes the mirror direction R as
//
//  R = 2 * dot(A,N) * N - A
//
inline void mirror( const Vector4 & a, const Vector4 & n, Vector4 & r )
{
    Vector4 NtwoAdotN;
    float twoAdotN = 2.0f * dot( a, n );
    scale( n, twoAdotN, NtwoAdotN );
    subtract( NtwoAdotN, a, r );
}

#endif