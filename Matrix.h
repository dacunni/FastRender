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
	
	void identity();
	
	void print();
	
	float data[16];
	
};

void mult( Matrix4x4 & A, Matrix4x4 & B, Matrix4x4 & R );
void scale( Matrix4x4 & A, float s, Matrix4x4 & R );
void inverse( Matrix4x4 & A, Matrix4x4 & R );


#endif
