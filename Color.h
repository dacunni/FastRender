/*
 *  Color.h
 *  FastRender
 *
 *  Created by David Cunningham on 4/12/14
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _COLOR_H_
#define _COLOR_H_


#define RED_BIT   0x1
#define GREEN_BIT 0x2
#define BLUE_BIT  0x4

// Mask indicating which wavelength(s) are we sampling {RED_BIT, GREEN_BIT, BLUE_BIT}
// Used by radiance sampling
typedef unsigned char RGBColorMask;

class RGBColor {
    public:
    RGBColor( float red, float green, float blue ) : r(red), g(green), b(blue) {}
    RGBColor() : r(0.0f), g(0.0f), b(0.0f) {}
    ~RGBColor() {}

    void setRGB( float red, float green, float blue ) { r = red; g = green; b = blue; }

    union {
        struct {
            float r, g, b;
        };
        float rgb[3];
    };
};

#endif
