/*
 *  Vector.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 7/21/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <math.h>
#include <stdio.h>

#include "Vector.h"

Vector4::Vector4( const Vector4 & a ) 
{
	data[0] = a.data[0];
	data[1] = a.data[1];
	data[2] = a.data[2];
	data[3] = a.data[3];
}

Vector4::Vector4( float x, float y, float z, float w ) 
{
	data[0] = x;
	data[1] = y;
	data[2] = z;
	data[3] = w;
}

float Vector4::magnitude_sq()
{
	return sq(data[0]) + sq(data[1]) + sq(data[2]);
}

float Vector4::magnitude()
{
	return sqrt( sq(data[0]) + sq(data[1]) + sq(data[2]) );
}

void Vector4::normalize()
{
	float mag = magnitude();
	
	if( mag != 0.0 ) {
		scale( *this, 1.0f / mag, *this );
	}
}

void Vector4::print()
{
	printf( "V4( %f %f %f %f )\n", data[0], data[1], data[2], data[3] );
}

void Vector4::fprintCSV( FILE * file )
{
	fprintf( file, "%f,%f,%f,%f\n", data[0], data[1], data[2], data[3] );
}

// TODO - Decide whether to normalize by 1/w before doing this
void dot( const Vector4 & a, const Vector4 & b, float & r )
{
	r = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

// TODO - Decide whether to normalize by 1/w before doing this
void cross( const Vector4 & a, const Vector4 & b, Vector4 & r )
{
	r[0] = a[1] * b[2] - a[2] * b[1];
	r[1] = a[2] * b[0] - a[0] * b[2];
	r[2] = a[0] * b[1] - a[1] * b[0];
}

void add( const Vector4 & a, const Vector4 & b, Vector4 & r )
{
	r[0] = a[0] + b[0];
	r[1] = a[1] + b[1];
	r[2] = a[2] + b[2];
	r[3] = 1.0;
}

void subtract( const Vector4 & a, const Vector4 & b, Vector4 & r )
{
	r[0] = a[0] - b[0];
	r[1] = a[1] - b[1];
	r[2] = a[2] - b[2];
	r[3] = 1.0;	
}

void scale( const Vector4 & a, float s, Vector4 & r )
{
	r[0] = a[0] * s;
	r[1] = a[1] * s;
	r[2] = a[2] * s;
	r[3] = a[3];
}

void perspective_scale( const Vector4 & a, Vector4 & r )
{
	if( a[3] != 0.0 ) {
		r[0] = a[0] / a[3];
		r[1] = a[1] / a[3];
		r[2] = a[2] / a[3];
		r[3] = 1.0;
	}
}

