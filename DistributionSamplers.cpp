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
    pdf = std::vector<float>(p, p+n);

    float sum = 0.0f;
    for( auto p : pdf ) {
        sum += p;
        cdf.push_back(sum);
    }
}

float DistributionSampler1D::sample()
{
    float u = rng.uniform01();

    // TODO: Binary search or something faster
    for( unsigned int i = 0; i < cdf.size(); ++i ) {
        if( cdf[i] > u ) {
            return (float) i / (cdf.size() - 1);
        }
    }

    return 0.0;
}


