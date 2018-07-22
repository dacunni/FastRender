#ifndef _COLOR_H_
#define _COLOR_H_

#include <stdio.h>
#include <iosfwd>
#include <cmath>

#define RED_BIT   0x1
#define GREEN_BIT 0x2
#define BLUE_BIT  0x4
#define RGB_BITS (RED_BIT | GREEN_BIT | BLUE_BIT)

class RGBColor {
    public:
    RGBColor( float red, float green, float blue ) : r(red), g(green), b(blue) {}
    RGBColor( float whiteness ) : r(whiteness), g(whiteness), b(whiteness) {}
    RGBColor( const RGBColor & c ) : r(c.r), g(c.g), b(c.b) {}
    RGBColor() : r(0.0f), g(0.0f), b(0.0f) {}
    ~RGBColor() {}

    void setRGB( float red, float green, float blue ) { r = red; g = green; b = blue; }
    void scale( float s ) { r *= s; g *= s; b *= s; }
    void scale( float rs, float gs, float bs ) { r *= rs; g *= gs; b *= bs; }
    RGBColor scaled( float s ) const { auto c(*this); c.scale(s); return c; }
    RGBColor scaled( float rs, float gs, float bs ) const { auto c(*this); c.scale(rs, gs, bs); return c; }
    void accum( const RGBColor & c ) { r += c.r; g += c.g; b += c.b; }

    inline RGBColor operator+=( const RGBColor & c ) { accum(c); return *this; }

    void print() const;

    bool isZero() const { return r == 0.0f && g == 0.0f && b == 0.0f; }
    bool isNonNegative() const { return r >= 0.0f && g >= 0.0f && b >= 0.0f; }
    bool isFinite() const { return std::isfinite(r) && std::isfinite(g) && std::isfinite(b); }

    float r, g, b;
};

inline RGBColor mult( const RGBColor & a, const RGBColor & b ) { return a.scaled(b.r, b.g, b.b); }
inline RGBColor mult( const RGBColor & a, float s ) { return a.scaled(s); }
inline RGBColor mult( float s, const RGBColor & a ) { return a.scaled(s); }

inline RGBColor operator+( const RGBColor & a, const RGBColor & b )
    { return RGBColor(a.r + b.r, a.g + b.g, a.b + b.b); }
inline RGBColor operator*( const RGBColor & a, const RGBColor & b ) { return a.scaled(b.r, b.g, b.b); }
inline RGBColor operator*( const RGBColor & a, float s ) { return a.scaled(s); }
inline RGBColor operator*( float s, const RGBColor & a ) { return a.scaled(s); }

std::ostream & operator<<( std::ostream & o, const RGBColor & c );

#endif
