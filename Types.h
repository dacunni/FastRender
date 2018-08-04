/*
 *  Types.h
 *  FastRender
 *
 *  Created by David Cunningham on 8/21/16
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _TYPES_H_
#define _TYPES_H_

template<typename SCALAR>
union ScalarArray2 {
    SCALAR data[2];
    struct { SCALAR x, y; };
    struct { SCALAR r, g; };
    struct { SCALAR u, v; };
    ScalarArray2(const ScalarArray2<SCALAR> &) = default;
    ScalarArray2(SCALAR _x, SCALAR _y) : x(_x), y(_y) {}
    SCALAR & operator[](unsigned int i) { return data[i]; }
    const SCALAR & operator[](unsigned int i) const { return data[i]; }
};

typedef ScalarArray2<int> int2;
typedef ScalarArray2<unsigned int> uint2;
typedef ScalarArray2<float> float2;
typedef ScalarArray2<double> double2;

template<typename SCALAR>
union ScalarArray3 {
    SCALAR data[3];
    struct { SCALAR x, y, z; };
    struct { SCALAR r, g, b; };
    struct { SCALAR u, v; };
    ScalarArray3(const ScalarArray3<SCALAR> &) = default;
    ScalarArray3(SCALAR _x, SCALAR _y, SCALAR _z) : x(_x), y(_y), z(_z) {}
    SCALAR & operator[](unsigned int i) { return data[i]; }
    const SCALAR & operator[](unsigned int i) const { return data[i]; }

    ScalarArray3 operator+(const ScalarArray3 & o)
        { return ScalarArray3(x + o.x, y + o.y, z + o.z); }
    ScalarArray3 operator+=(const ScalarArray3 & o)
        { *this = *this + o; return *this; }

    ScalarArray3 operator*(const SCALAR m)
        { return ScalarArray3(x * m, y * m, z * m); }
    ScalarArray3 operator*=(const SCALAR m)
        { *this = *this * m; return *this; }

    ScalarArray3 operator/(const SCALAR d)
        { return ScalarArray3(x / d, y / d, z / d); }
    ScalarArray3 operator/=(const SCALAR d)
        { *this = *this / d; return *this; }
};

typedef ScalarArray3<int> int3;
typedef ScalarArray3<unsigned int> uint3;
typedef ScalarArray3<float> float3;
typedef ScalarArray3<double> double3;

template<typename SCALAR>
union ScalarArray4 {
    SCALAR data[4];
    struct { SCALAR x, y, z, w; };
    struct { SCALAR r, g, b, a; };
    struct { SCALAR u, v; };
    ScalarArray4(const ScalarArray4<SCALAR> &) = default;
    ScalarArray4(SCALAR _x, SCALAR _y, SCALAR _z, SCALAR _w) : x(_x), y(_y), z(_z), w(_w) {}
    SCALAR & operator[](unsigned int i) { return data[i]; }
    const SCALAR & operator[](unsigned int i) const { return data[i]; }
};

typedef ScalarArray4<int> int4;
typedef ScalarArray4<unsigned int> uint4;
typedef ScalarArray4<float> float4;
typedef ScalarArray4<double> double4;



#endif
