
#include <iostream>
#include <iomanip>
#include "Color.h"

void RGBColor::print() const
{
    printf("RGBColor( %0.3f %0.3f %0.3f )\n", r, g, b );
}

std::ostream & operator<<( std::ostream & o, const RGBColor & c )
{
    return o 
        << std::setprecision(5)
        << "RGB(" << c.r << ", " << c.g << ", " << c.b << ")";
}
