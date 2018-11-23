#include <math.h>
#include <stdio.h>

#include "Quaternion.h"

void Quaternion::print() const
{
    printf( "Qt( %f %f %f %f )\n", data[0], data[1], data[2], data[3] );
}

void Quaternion::fprintCSV( FILE * file ) const
{
    fprintf( file, "%f,%f,%f,%f\n", data[0], data[1], data[2], data[3] );
}



