/*
 *  DistributionSamplers.h
 *  FastRender
 *
 *  Created by David Cunningham on 8/14/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _DISTRIBUTION_SAMPLERS_
#define _DISTRIBUTION_SAMPLERS_

#include <vector>

class RandomNumberGenerator;

class DistributionSampler1D
{
public:
	DistributionSampler1D( RandomNumberGenerator & r, float * p, unsigned int n );
	~DistributionSampler1D();

    void setPDF( float * p, unsigned int n );
    float cdfAt( int i ) { return cdf[i]; }
    float sample();
	
private:
    RandomNumberGenerator & rng;
    std::vector<float> pdf;
    std::vector<float> cdf;
};



#endif


