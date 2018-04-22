/*
 *  DistributionSamplers.cpp
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <numeric>
#include <algorithm>

#include "RandomNumberGenerator.h"
#include "DistributionSamplers.h"

DistributionSampler1D::DistributionSampler1D( RandomNumberGenerator & r, float * p, unsigned int n )
    : rng(r)
{
    setPDF( p, n );
}

DistributionSampler1D::~DistributionSampler1D()
{

}

void DistributionSampler1D::setPDF( float * p, unsigned int n )
{
    pdf.resize(n);
    std::copy(p, p + n, pdf.begin());

    float sum = 0.0f;
    for( auto prob : pdf ) {
        sum += prob;
        cdf.push_back(sum);
    }
    // FIXME: Do we need to normalize?
}

DistributionSampler1D::Sample DistributionSampler1D::sample()
{
    float u = rng.uniform01();

    // TODO: Binary search or something faster
    // TODO: Interpolate
    for( unsigned int i = 0; i < cdf.size(); ++i ) {
        if( cdf[i] > u ) {
            return { .value = (float) i / (cdf.size() - 1),
                     .pdf = pdf[i] };
        }
    }

    return { .value = 0.0f, .pdf = 0.0f }; // FIXME: Is this okay?
}


DistributionSampler2D::DistributionSampler2D( RandomNumberGenerator & r, float * p, unsigned int w, unsigned int h )
    : rng(r), width(w), height(h)
{
    setPDF( p, w, h );
}

DistributionSampler2D::~DistributionSampler2D()
{

}

void DistributionSampler2D::setPDF( float * p, unsigned int w, unsigned int h )
{
    width = w;
    height = h;
    pdf.resize(w * h);
    std::copy(p, p + w * h, pdf.begin());
    float sum = std::accumulate(pdf.begin(), pdf.end(), 0.0f);
    std::transform(pdf.begin(), pdf.end(), pdf.begin(),
                   [sum](float x){ return x / sum; });
    cdf_u_given_v.resize(w * h);
    cdf_v.resize(w);

    float sum_along_v = 0.0f;
    for( unsigned int v = 0; v < h; v++ ) {
        // Create cumulative sums along u
        float sum_along_u = 0.0f;
        for( unsigned int u = 0; u < w; u++ ) {
            auto index = uvToIndex(u, v);
            sum_along_u += pdf[index];
            cdf_u_given_v[index] = sum_along_u;
        }
        // Normalize cumulative sums to [0, 1]
        auto row_start = cdf_u_given_v.begin() + uvToIndex(0, v);
        auto row_end   = cdf_u_given_v.begin() + uvToIndex(width, v);
        std::transform(row_start, row_end, row_start,
                       [sum_along_u](float x){ return x / sum_along_u; });

        sum_along_v += sum_along_u;
        // Create cumulative sum along v
        cdf_v[v] = sum_along_v;
    }
    // Normalize cumulative sum along u to [0, 1]
    std::transform(cdf_v.begin(), cdf_v.end(), cdf_v.begin(),
                   [sum_along_v](float x){ return x / sum_along_v; });

    // TEMP >>>
#if 0
    float psum = std::accumulate(pdf.begin(), pdf.end(), 0.0f);
    float pmax = *std::max_element(pdf.begin(), pdf.end());
    float csum = std::accumulate(cdf_v.begin(), cdf_v.end(), 0.0f);
    float cmax = *std::max_element(cdf_v.begin(), cdf_v.end());
    printf("psum=%f pmax=%f csum=%f cmax=%f sum_along_v=%f\n",
           psum, pmax, csum, cmax, sum_along_v);
#endif
    // TEMP <<<
}

DistributionSampler2D::Sample DistributionSampler2D::sample() const
{
    float e1 = rng.uniform01();
    float e2 = rng.uniform01();

    // TODO: Interpolate

    // Find v using CDF(v) -> CDF(v) > e1
    auto vi = std::upper_bound(cdf_v.begin(), cdf_v.end(), e1);
    unsigned int v = (vi == cdf_v.end() ? height - 1 : vi - cdf_v.begin());

    // Find u using CDF(u|v) -> CDF(u|v) > e2
    auto row_start = cdf_u_given_v.begin() + uvToIndex(0, v);
    auto row_end   = cdf_u_given_v.begin() + uvToIndex(width, v);
    auto ui = std::upper_bound(row_start, row_end, e2);
    unsigned int u = (ui == row_end ? width - 1 : ui - row_start);
    
    return {
        .u = (float) u / (width - 1),
        .v = (float) v / (height - 1),
        // FIXME: Is this the right thing to do?
        // Transform discrete PDF to real PDF
        .pdf = pdf[uvToIndex(u, v)] * width * height
    };
}


