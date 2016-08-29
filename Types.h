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
union ScalarArray3 {
    SCALAR data[3];
    struct { SCALAR x, y, z; }; 
    struct { SCALAR r, g, b; }; 
    struct { SCALAR u, v; }; 
    SCALAR & operator[](unsigned int i) { return data[i]; }
    const SCALAR & operator[](unsigned int i) const { return data[i]; }
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
    SCALAR & operator[](unsigned int i) { return data[i]; }
    const SCALAR & operator[](unsigned int i) const { return data[i]; }
};

typedef ScalarArray4<int> int4;
typedef ScalarArray4<unsigned int> uint4;
typedef ScalarArray4<float> float4;
typedef ScalarArray4<double> double4;



#endif
