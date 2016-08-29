//
//  Quaternion.hpp
//  FastRender
//
//  Created by David Cunningham on 8/21/2016.
//
//

#ifndef FastRender_Vector_hpp
#define FastRender_Vector_hpp

#include <cmath>
#include "GeometryUtils.h"

inline Quaternion::Quaternion()
    : x(0.0f), y(0.0f), z(0.0f), r(1.0f)
{

}

inline Quaternion::Quaternion( const Quaternion & a )
{
	data[0] = a.data[0];
	data[1] = a.data[1];
	data[2] = a.data[2];
	data[3] = a.data[3];
}

inline Quaternion::Quaternion( float xn, float yn, float zn, float rn )
{
	data[0] = xn;
	data[1] = yn;
	data[2] = zn;
	data[3] = rn;
}

inline void Quaternion::set( float xn, float yn, float zn, float rn )
{
	data[0] = xn;
	data[1] = yn;
	data[2] = zn;
	data[3] = rn;
}

inline void       mult( const Quaternion & q, const Quaternion & p, Quaternion & r )
{
    // IMPLEMENT ME
}

inline Quaternion mult( const Quaternion & q, const Quaternion & p )
{
    // IMPLEMENT ME
}

inline void       rotate( const Quaternion & q, const Vector4 & v, Vector4 & r )
{
    // IMPLEMENT ME
}

inline Quaternion rotate( const Quaternion & q, const Vector4 & v )
{
    // IMPLEMENT ME
}




#endif
