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

#include <iostream>
#include <fstream>
#include <stdio.h>

template< typename T > inline T sq( T & t ) { return t * t; }

class Vector4 
{
public:
	Vector4() {}	
	Vector4( const Vector4 & a );
	Vector4( float x, float y, float z, float w = 1.0f );
	~Vector4() {}
	
	inline float & operator[]( int i ) 
	{ 
		return data[i]; 
	}

	inline const float & operator[]( int i ) const
	{ 
		return data[i]; 
	}
	
    float x() const { return data[0]; }
    float y() const { return data[1]; }
    float z() const { return data[2]; }
    float w() const { return data[3]; }
    
	float magnitude_sq();
	float magnitude();
	void normalize();
	
	void print();
    void fprintCSV( FILE * file );
	
	float data[4];
};

void dot( const Vector4 & a, const Vector4 & b, float & r );
void cross( const Vector4 & a, const Vector4 & b, Vector4 & r );
void add( const Vector4 & a, const Vector4 & b, Vector4 & r );
void subtract( const Vector4 & a, const Vector4 & b, Vector4 & r );
void scale( const Vector4 & a, float s, Vector4 & r );
void perspective_scale( const Vector4 & a, Vector4 & r );



#endif
