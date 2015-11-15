/*
 *  Vector.h
 *  FastRender
 *
 *  Created by David Cunningham on 7/21/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <fstream>
#include <stdio.h>

template< typename T > inline T sq( const T & t ) { return t * t; }

class Vector4 
{
public:
	inline Vector4();
	inline Vector4( const Vector4 & a );
	inline Vector4( float x, float y, float z, float w = 1.0f );
	inline ~Vector4() {}
	
	inline float & operator[]( int i ) 
	{ 
		return data[i]; 
	}

	inline const float & operator[]( int i ) const
	{ 
		return data[i]; 
	}
	
    inline void set( float x, float y, float z, float w = 1.0f );
    
	float magnitude_sq();
	float magnitude();
	void normalize();
    void negate();

	Vector4 normalized();
	Vector4 negated();
	
	void print() const;
    void fprintCSV( FILE * file ) const;
	
    union {
	float data[4];
        struct {
            float x, y, z, w;
        };
    };
};

// Vector math functions
//   Includes in-place and algebraic forms for convenience
inline void    dot( const Vector4 & a, const Vector4 & b, float & r );
inline float   dot( const Vector4 & a, const Vector4 & b );
inline void    cross( const Vector4 & a, const Vector4 & b, Vector4 & r );
inline Vector4 cross( const Vector4 & a, const Vector4 & b );
inline void    add( const Vector4 & a, const Vector4 & b, Vector4 & r );
inline Vector4 add( const Vector4 & a, const Vector4 & b );
inline void    subtract( const Vector4 & a, const Vector4 & b, Vector4 & r );
inline Vector4 subtract( const Vector4 & a, const Vector4 & b );
inline void    scale( const Vector4 & a, float s, Vector4 & r );
inline Vector4 scale( const Vector4 & a, float s );
inline void    perspective_scale( const Vector4 & a, Vector4 & r );
inline Vector4 perspective_scale( const Vector4 & a );
inline void    mirror( const Vector4 & a, const Vector4 & n, Vector4 & r );
inline Vector4 mirror( const Vector4 & a, const Vector4 & n );
inline void    interp( const Vector4 & a, const Vector4 & b, const float alpha, Vector4 & r);
inline Vector4 interp( const Vector4 & a, const Vector4 & b, const float alpha );

#include "Vector.hpp"

#endif
