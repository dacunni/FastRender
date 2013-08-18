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

template< typename T > inline T sq( T & t ) { return t * t; }

class Vector4 
{
public:
	inline Vector4() {}
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
    
    inline float x() const { return data[0]; }
    inline float y() const { return data[1]; }
    inline float z() const { return data[2]; }
    inline float w() const { return data[3]; }
    
	float magnitude_sq();
	float magnitude();
	void normalize();
    void negate();
	
	void print() const;
    void fprintCSV( FILE * file ) const;
	
	float data[4];
};

inline void dot( const Vector4 & a, const Vector4 & b, float & r );
inline float dot( const Vector4 & a, const Vector4 & b );
inline void cross( const Vector4 & a, const Vector4 & b, Vector4 & r );
inline void add( const Vector4 & a, const Vector4 & b, Vector4 & r );
inline void subtract( const Vector4 & a, const Vector4 & b, Vector4 & r );
inline void scale( const Vector4 & a, float s, Vector4 & r );
inline void perspective_scale( const Vector4 & a, Vector4 & r );

#include "Vector.hpp"

#endif
