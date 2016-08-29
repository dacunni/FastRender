/*
 *  Quaternion.h
 *  FastRender
 *
 *  Created by David Cunningham on 8/32/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include <fstream>
#include <stdio.h>
#include "Vector.h"

class Quaternion 
{
public:
	inline Quaternion();
	inline Quaternion( const Vector4 & a );
	inline Quaternion( float x, float y, float z, float w = 1.0f );
	inline ~Quaternion() {}
	
	inline float & operator[]( int i ) 
	{ 
		return data[i]; 
	}

	inline const float & operator[]( int i ) const
	{ 
		return data[i]; 
	}
	
    // IMPLEMENT ME
    inline void set( float x, float y, float z, float r );
    // IMPLEMENT ME
    inline Quaternion conjugate();
    
	void print() const;
    void fprintCSV( FILE * file ) const;

    union {
        // array format (vector components first)
	    float data[4];
        // named components
        struct {
            float x, y, z, r;
        };
        // vector + scalar format
        struct {
            float v[3];
            float _;
        };
    };
};

// Quaternion math functions
//   Includes in-place and algebraic forms for convenience
inline void       mult( const Quaternion & q, const Quaternion & p, Quaternion & r );
inline Quaternion mult( const Quaternion & q, const Quaternion & p );
inline void       rotate( const Quaternion & q, const Vector4 & v, Vector4 & r );
inline Quaternion rotate( const Quaternion & q, const Vector4 & v );

#include "Quaternion.hpp"

#endif
