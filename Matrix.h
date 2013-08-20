/*
 *  Matrix.h
 *  FastRender
 *
 *  Created by David Cunningham on 7/21/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "Vector.h"

//
// 4x4 projection matrix. Always assumed to be of the form
//
//  [ R T ]
//  [ 0 1 ]
//
//  Where R is a rotation matrix, and T is a translation vector
//
class Matrix4x4 
{
public:
	Matrix4x4() {}
	Matrix4x4( float d00, float d01, float d02, float d03,
			   float d10, float d11, float d12, float d13,
			   float d20, float d21, float d22, float d23,
               float d30, float d31, float d32, float d33 );
	~Matrix4x4() {}
	
	inline float & at( int r, int c ) 
	{ 
		return data[r * 4 + c]; 
	}

	inline float at( int r, int c ) const
	{
		return data[r * 4 + c];
	}
    
	void identity();
	
	void print();
	
	float data[16];
	
};

// Computes the matrix R = A * B
void mult( const Matrix4x4 & A, const Matrix4x4 & B, Matrix4x4 & R );

// Multiplies a matrix by a vector. Assumes that matrices multiply on the left of vectors.
// r = A * v
void mult( const Matrix4x4 & A, const Vector4 & v, Vector4 & r );

void scale( const Matrix4x4 & A, float s, Matrix4x4 & R );

// FIXME - not really an inverse unless you make assumptions about the matrix
void inverse( const Matrix4x4 & A, Matrix4x4 & R );


#endif
