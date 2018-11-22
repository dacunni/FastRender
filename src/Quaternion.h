#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include <iosfwd>
#include <stdio.h>
#include "Vector.h"

class Quaternion 
{
    public:
        inline Quaternion();
        inline Quaternion( const Vector4 & a );
        inline Quaternion( float x, float y, float z, float r = 1.0f );
        inline Quaternion( const Quaternion & a );
        inline ~Quaternion() {}

        inline       float & operator[]( int i )       { return data[i]; }
        inline const float & operator[]( int i ) const { return data[i]; }

        inline void set( float x, float y, float z, float r );

        inline Quaternion conjugate();

        void print() const;
        void fprintCSV( FILE * file ) const;

        union {
            float data[4]; // array format (vector components first)
            struct { float x, y, z, r; };    // named components
            struct { float v[3],  _; }; // vector + scalar format
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
