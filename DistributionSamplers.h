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

    struct Sample {
        float value;
        float pdf;
    };

    void setPDF( float * p, unsigned int n );
    inline float cdfAt( int i ) { return cdf[i]; }
    Sample sample();
	
private:
    RandomNumberGenerator & rng;
    std::vector<float> pdf;
    std::vector<float> cdf;
};

class DistributionSampler2D
{
public:
	DistributionSampler2D( RandomNumberGenerator & r,
                           float * p, unsigned int w, unsigned int h );
	~DistributionSampler2D();

    struct Sample {
        float u, v;
        float pdf;
    };

    void setPDF( float * p, unsigned int w, unsigned int h );
    inline float pdfAt( int u, int v ) const { return pdf[uvToIndex(u, v)]; }
    inline float cdfAt( int u, int v ) const { return cdf_u_given_v[uvToIndex(u, v)]; }
    inline float cdfAt( int v ) const { return cdf_v[v]; }
    Sample sample() const;
	
private:
    // Raster is row major
    // CDF(v) is used to pick row, then CDF(u|v) is used to
    // pick column within row
    inline int uvToIndex( int u, int v ) const { return v * width + u; }

    RandomNumberGenerator & rng;
    std::vector<float> pdf;
    std::vector<float> cdf_u_given_v;
    std::vector<float> cdf_v;
    unsigned int width;
    unsigned int height;
};


#endif


