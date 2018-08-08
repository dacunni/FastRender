#include <math.h>
#include <stdio.h>
#include <cassert>
#include <sstream>

#include "Matrix.h"

Matrix4x4::Matrix4x4( float d00, float d01, float d02, float d03,
					  float d10, float d11, float d12, float d13,
					  float d20, float d21, float d22, float d23,
					  float d30, float d31, float d32, float d33 ) 
{
	at( 0, 0 ) = d00;
	at( 0, 1 ) = d01;
	at( 0, 2 ) = d02;
	at( 0, 3 ) = d03;
	at( 1, 0 ) = d10;
	at( 1, 1 ) = d11;
	at( 1, 2 ) = d12;
	at( 1, 3 ) = d13;
	at( 2, 0 ) = d20;
	at( 2, 1 ) = d21;
	at( 2, 2 ) = d22;
	at( 2, 3 ) = d23;
	at( 3, 0 ) = d30;
	at( 3, 1 ) = d31;
	at( 3, 2 ) = d32;
	at( 3, 3 ) = d33;		
}

void Matrix4x4::identity() 
{
	for( int r = 0; r < 4; r++ ) {
		for( int c = 0; c < 4; c++ ) {
			at( r, c ) = (r == c ? 1.0 : 0.0);
		}
	}
}

// Symmetric frustum OpenGL projection matrix construction
// Based on "OpenGL Projection Matrix" article at
// http://www.songho.ca/opengl/gl_projectionmatrix.html
void Matrix4x4::glProjectionSymmetric( float width, float height, float near, float far )
{
    if( width < 0.0f )
        width = -width;
    if( height < 0.0f )
        height = -height;
    if( far < near )
        std::swap( near, far );

    identity();
    at( 0, 0 ) = near / (0.5 * width);
    at( 1, 1 ) = near / (0.5 * height);
    at( 2, 2 ) = -(far + near) / (far - near);
    at( 2, 3 ) = -2.0f * far * near / (far - near);
    at( 3, 2 ) = -1.0f;
    at( 3, 3 ) = 0.0f;
}

// General purpose OpenGL projection matrix construction
// Based on "OpenGL Projection Matrix" article at
// http://www.songho.ca/opengl/gl_projectionmatrix.html
// FIXME - something doesn't seem quite right here
//         it behaves symmetrically in y even when top != -bottom
void Matrix4x4::glProjection( float left, float right, 
                              float bottom, float top, 
                              float near, float far )
{
    if( right < left )
        std::swap( left, right );
    if( top < bottom )
        std::swap( bottom, top );
    if( far < near )
        std::swap( near, far );

    identity();
    at( 0, 0 ) = 2.0f * near / (right - left);
    at( 0, 2 ) = (right + left) / (right - left);
    at( 1, 1 ) = 2.0f * near / (top - bottom);
    at( 1, 2 ) = (top + bottom) / (top - bottom);
    at( 2, 2 ) = -(far + near) / (far - near);
    at( 2, 3 ) = -2.0f * far * near / (far - near);
    at( 3, 2 ) = -1.0f;
    at( 3, 3 ) = 0.0f;
}

void Matrix4x4::print() const
{
	for( int r = 0; r < 4; r++ ) {
		printf( "M%d| %6.6f %6.6f %6.6f %6.6f |\n", r, at(r, 0), at(r, 1), at(r, 2), at( r, 3) );
	}
}

std::string Matrix4x4::toString() const
{
    std::stringstream ss;
    ss << at(0,0) << "," << at(0,1) << "," << at(0,2) << "," << at(0,3) << "; "
       << at(1,0) << "," << at(1,1) << "," << at(1,2) << "," << at(1,3) << "; "
       << at(2,0) << "," << at(2,1) << "," << at(2,2) << "," << at(2,3) << "; "
       << at(3,0) << "," << at(3,1) << "," << at(3,2) << "," << at(3,3);
    return ss.str();
}

std::string Matrix4x4::toJSON() const
{
    std::stringstream ss;

    ss << "["
        << "[" << at(0,0) << "," << at(0,1) << "," << at(0,2) << "," << at(0,3) << "],"
        << "[" << at(1,0) << "," << at(1,1) << "," << at(1,2) << "," << at(1,3) << "],"
        << "[" << at(2,0) << "," << at(2,1) << "," << at(2,2) << "," << at(2,3) << "],"
        << "[" << at(3,0) << "," << at(3,1) << "," << at(3,2) << "," << at(3,3) << "]"
        << "]";
    
    return ss.str();
}

// TODO - handle R being the same as one of A or B
void mult( const Matrix4x4 & A, const Matrix4x4 & B, Matrix4x4 & R )
{
    assert(&A != &R && &B != &R);

	for( int r = 0; r < 4; r++ ) {
		for( int c = 0; c < 4; c++ ) {
			R.at( r, c ) = A.at( r, 0 ) * B.at( 0, c )
						 + A.at( r, 1 ) * B.at( 1, c )
						 + A.at( r, 2 ) * B.at( 2, c )
						 + A.at( r, 3 ) * B.at( 3, c );
		}
	}
}

Matrix4x4 mult( const Matrix4x4 & A, const Matrix4x4 & B )
{
    Matrix4x4 R;

    mult( A, B, R );

    return R;
}

// TODO - handle result being the same vector as v
void mult( const Matrix4x4 & A, const Vector4 & v, Vector4 & result )
{
    for( int r = 0; r < 4; r++ ) {
        result[r] = 0.0;
		for( int c = 0; c < 4; c++ ) {
			result[r] += A.at( r, c ) * v[c];
		}
	}
}

Vector4 mult( const Matrix4x4 & A, const Vector4 & v )
{
    Vector4 result;

    mult( A, v, result );

    return result;
}

void scale( const Matrix4x4 & A, float s, Matrix4x4 & R )
{
	for( int r = 0; r < 4; r++ ) {
		for( int c = 0; c < 4; c++ ) {
			R.at( r, c ) = A.at( r, c ) * s;
		}
	}	
}

Matrix4x4 scale( const Matrix4x4 & A, float s )
{
    Matrix4x4 R;

    scale( A, s, R );

    return R;
}

//
// Fast matrix inversion 
//
// Based on the fast matrix inversion tutortial here:
//		http://content.gpwiki.org/index.php/MathGem:Fast_Matrix_Inversion
//
void inverse( const Matrix4x4 & A, Matrix4x4 & R )
{
	// Transpose the 3x3 rotation submatrix
	R.at( 0, 0 ) = A.at( 0, 0 );
	R.at( 0, 1 ) = A.at( 1, 0 );
	R.at( 0, 2 ) = A.at( 2, 0 );
	R.at( 1, 0 ) = A.at( 0, 1 );
	R.at( 1, 1 ) = A.at( 1, 1 );
	R.at( 1, 2 ) = A.at( 2, 1 );
	R.at( 2, 0 ) = A.at( 0, 2 );
	R.at( 2, 1 ) = A.at( 1, 2 );
	R.at( 2, 2 ) = A.at( 2, 2 );
	
	// Last column translation vector
	//   Each element is the negative of a column vector of the rotation submatrix dotted with the translation vector
	R.at( 0, 3 ) = - (A.at( 0, 0 ) * A.at( 0, 3 ) + A.at( 1, 0 ) * A.at( 1, 3 ) + A.at( 2, 0 ) * A.at( 2, 3 ));
	R.at( 1, 3 ) = - (A.at( 0, 1 ) * A.at( 0, 3 ) + A.at( 1, 1 ) * A.at( 1, 3 ) + A.at( 2, 1 ) * A.at( 2, 3 ));
	R.at( 2, 3 ) = - (A.at( 0, 2 ) * A.at( 0, 3 ) + A.at( 1, 2 ) * A.at( 1, 3 ) + A.at( 2, 2 ) * A.at( 2, 3 ));
	
	// Last row
	R.at( 3, 0 ) = 0.0f;
	R.at( 3, 1 ) = 0.0f;
	R.at( 3, 2 ) = 0.0f;
	R.at( 3, 3 ) = 1.0f;
}

Matrix4x4 inverse( const Matrix4x4 & A )
{
    Matrix4x4 R;

    inverse( A, R );

    return R;
}


