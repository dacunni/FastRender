#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>

#include <sys/stat.h>
#include <stdio.h>
#include <math.h>

#include "FastRender.h"

RandomNumberGenerator rng;
std::string output_path = "testoutput";
unsigned int plot_size = 500;

void testSamplePDF1D()
{
    // make a random pdf
    std::vector<float> pdf(20);
    for( auto & p : pdf ) {
        p = rng.uniform01();
    }
    // normalize the pdf
    float sum = 0;
    for( auto p : pdf ) { sum += p; }
    for( auto & p : pdf ) { p /= sum; }

    DistributionSampler1D sampler( rng, &pdf[0], pdf.size() );
    const unsigned int len = pdf.size();

    Plot2D plot( output_path + "/sample_pdf_1d.png", plot_size, plot_size,
                 0.0, len, 0.0, *std::max_element(pdf.begin(), pdf.end()) );
    plot.strokeColor(0, 0, 0);

    // draw the pdf
    for( int i = 0; i < len; ++i ) {
        plot.fillColor(0, 0, 1);
        plot.drawRect( i, 0, i+1, pdf[i] );
    }

    // sample the pdf
    std::vector<float> sample_dist(len);
    for( int i = 0; i < 10000; i++ ) {
        float v = sampler.sample();
        unsigned int index = (unsigned int) (v * (len - 1));
        index = std::min(index, len - 1);
        sample_dist[index] += 1.0f;
    }

    // normalize the sample distribution
    sum = 0;
    for( auto s : sample_dist ) { sum += s; }
    for( auto & s : sample_dist ) { s /= sum; }

    // draw the sample distribution
    for( int i = 0; i < len; ++i ) {
        plot.fillColor(1, 0, 0);
        plot.drawRect( i+0.3, 0, i+1-0.3, sample_dist[i] );
    }
}

int main (int argc, char * const argv[]) 
{
    printf("RNG Tests\n");
    fflush(stdout);

    mkdir(output_path.c_str(), 0777);

    Timer total_run_timer;
    total_run_timer.start();

    rng.seedCurrentTime();

    testSamplePDF1D();
    
    total_run_timer.stop();
    printf("Done - Run time = %f seconds\n", total_run_timer.elapsed());
    fflush(stdout);
    return 0;
}

