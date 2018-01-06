#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <iosfwd>
#include <cassert>
#include <stdio.h>

template< typename T > inline T sq( const T & t ) { return t * t; }

class Vector4 
{
public:
	inline Vector4();
	inline Vector4( const Vector4 & a );
	inline Vector4( float x, float y, float z, float w = 1.0f );
	inline ~Vector4() {}
	
	inline float & operator[]( int i );
	inline const float & operator[]( int i ) const;

    inline bool operator==( const Vector4 & o ) const {
        return (w == 0.0f && x == o.x && y == o.y && z == o.z) ||
               (x/w == o.x/o.w && y/w == o.y/o.w && z/w == o.z/o.w);
    }
	
    inline void set( float x, float y, float z, float w = 1.0f );
    
	inline float magnitude_sq() const;
	inline float magnitude() const;
	inline void normalize();
    inline void negate();
    inline void makeDirection() { w = 0.0f; }

	inline Vector4 normalized() const;
	inline Vector4 negated() const;
    inline bool isUnity() const;
    inline bool isDirection() const;
	
	void print() const;
    void fprintCSV( FILE * file ) const;
    std::string toJSON() const;

    // Assert methods
    void assertIsUnity() const { assert( isUnity() ); }
    void assertIsDirection() const { assert( isDirection() ); }

    union {
    	float data[4];
        struct {
            float x, y, z, w;
        };
    };
};

std::ostream & operator<<( std::ostream & os, const Vector4 & v );

// Vector math functions
//   Includes in-place and algebraic forms for convenience
inline void    dot( const Vector4 & a, const Vector4 & b, float & r );
inline float   dot( const Vector4 & a, const Vector4 & b );

inline float   clampedDot( const Vector4 & a, const Vector4 & b );

inline void    cross( const Vector4 & a, const Vector4 & b, Vector4 & r );
inline Vector4 cross( const Vector4 & a, const Vector4 & b );

inline void    add( const Vector4 & a, const Vector4 & b, Vector4 & r );
inline Vector4 add( const Vector4 & a, const Vector4 & b );
inline Vector4 operator+( const Vector4 & a, const Vector4 & b ) { return add(a, b); }

inline void    subtract( const Vector4 & a, const Vector4 & b, Vector4 & r );
inline Vector4 subtract( const Vector4 & a, const Vector4 & b );
inline Vector4 operator-( const Vector4 & a, const Vector4 & b ) { return subtract(a, b); }

inline void    scale( const Vector4 & a, float s, Vector4 & r );
inline Vector4 scale( const Vector4 & a, float s );
inline Vector4 operator*( const Vector4 & a, float s ) { return scale(a, s); }
inline Vector4 operator*( float s, const Vector4 & a ) { return scale(a, s); }

inline Vector4 blend( const Vector4 & a, float s, const Vector4 & b, float t );

inline void    perspective_scale( const Vector4 & a, Vector4 & r );
inline Vector4 perspective_scale( const Vector4 & a );

inline void    mirror( const Vector4 & a, const Vector4 & n, Vector4 & r );
inline Vector4 mirror( const Vector4 & a, const Vector4 & n );

inline void    refract( const Vector4 & a, const Vector4 & n, float n1, float n2, Vector4 & r );
inline Vector4 refract( const Vector4 & a, const Vector4 & n, float n1, float n2 );

inline void    interp( const Vector4 & a, const Vector4 & b, const float alpha, Vector4 & r);
inline Vector4 interp( const Vector4 & a, const Vector4 & b, const float alpha );

#include "Vector.hpp"

#endif
