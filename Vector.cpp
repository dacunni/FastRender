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
#include <sstream>

#include "Vector.h"

void Vector4::print() const
{
	printf( "V4( %f %f %f %f )\n", data[0], data[1], data[2], data[3] );
}

void Vector4::fprintCSV( FILE * file ) const
{
	fprintf( file, "%f,%f,%f,%f\n", data[0], data[1], data[2], data[3] );
}

std::string Vector4::toJSON() const
{
    std::stringstream ss;

    ss << "[" << x << "," << y << "," << z << "," << w << "]";
    
    return ss.str();
}


