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

#include <stdio.h>

#define RED_BIT   0x1
#define GREEN_BIT 0x2
#define BLUE_BIT  0x4
#define RGB_BITS (RED_BIT | GREEN_BIT | BLUE_BIT)

class RGBColor {
    public:
    RGBColor( float red, float green, float blue ) : r(red), g(green), b(blue) {}
    RGBColor( const RGBColor & c ) : r(c.r), g(c.g), b(c.b) {}
    RGBColor() : r(0.0f), g(0.0f), b(0.0f) {}
    ~RGBColor() {}

    void setRGB( float red, float green, float blue ) { r = red; g = green; b = blue; }
    void scale( float s ) { r *= s; g *= s; b *= s; }
    void scale( float rs, float gs, float bs ) { r *= rs; g *= gs; b *= bs; }
    RGBColor scaled( float s ) const { auto c(*this); c.scale(s); return c; }
    RGBColor scaled( float rs, float gs, float bs ) const { auto c(*this); c.scale(rs, gs, bs); return c; }
    void accum( const RGBColor & c ) { r += c.r; g += c.g; b += c.b; }

    void print() { printf("RGBColor( %0.3f %0.3f %0.3f )\n", r, g, b ); }

    bool isZero() { return r == 0.0f && g == 0.0f && b == 0.0f; }

    union {
        struct {
            float r, g, b;
        };
        float rgb[3];
    };
};

inline RGBColor mult( const RGBColor & a, const RGBColor & b ) {
    return RGBColor( a.r * b.r, a.g * b.g, a.b * b.b );
}

#endif
