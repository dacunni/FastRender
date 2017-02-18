#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>
#include <numeric>
#include <memory>

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
        //float v = sampler.sample();
        DistributionSampler1D::Sample sample = sampler.sample();
        float v = sample.value;
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

void testSamplePDF2D()
{
    // make a random pdf

    // Create random values and normalize them
#if 1
    // From an image
    unsigned int width = 1000, height = 1000;
    std::vector<float> pdf(width * height);
    HDRImage image("light_probes/debevec/grace_probe.float", width, height);
    image.maskUV([](float u, float v) { u -= 0.5f; v -= 0.5f; float r = 0.5f; return u*u + v*v <= r*r; });
    image.toGray(pdf);
#else
    // Random values
    unsigned int width = 40, height = 20;
    std::vector<float> pdf(width * height);
    std::transform(pdf.begin(), pdf.end(), pdf.begin(), [](float x){ return rng.uniform01(); });
#endif
    float sum = std::accumulate(pdf.begin(), pdf.end(), 0.0f);
    std::transform(pdf.begin(), pdf.end(), pdf.begin(), [sum](float x){ return x / sum; });
    float pdfmax = *std::max_element(pdf.begin(), pdf.end());
    printf("sum=%f pdfmax=%f\n", sum, pdfmax);

    DistributionSampler2D sampler( rng, &pdf[0], width, height );

    unsigned short pixel_max = (1 << MAGICKCORE_QUANTUM_DEPTH) - 1;
    std::unique_ptr<Magick::Image> pdf_image( new Magick::Image( Magick::Geometry( width, height ), "black" ) );
    pdf_image->magick( "png" ); // set the output file type
    Magick::PixelPacket * pdf_cache = pdf_image->setPixels( 0, 0, width, height );
    //std::unique_ptr<Magick::Image> cdf_image( new Magick::Image( Magick::Geometry( width + 2, height ), "black" ) );
    std::unique_ptr<Magick::Image> cdf_image( new Magick::Image( Magick::Geometry( width, height ), "black" ) );
    cdf_image->magick( "png" ); // set the output file type
    //Magick::PixelPacket * cdf_cache = cdf_image->setPixels( 0, 0, width + 2, height );
    Magick::PixelPacket * cdf_cache = cdf_image->setPixels( 0, 0, width, height );

    printf("Outputting PDF, CDF plots\n");
    float f;
    for( unsigned int u = 0; u < width; u++ ) {
        for( unsigned int v = 0; v < height; v++ ) {
            f = sampler.pdfAt( u, v ) / pdfmax * pixel_max;;
            f *= 1000.0f; // TEMP
            f = std::min(f, (float)pixel_max);
            pdf_cache[ v * width + u ].red   = f;
            pdf_cache[ v * width + u ].green = f;
            pdf_cache[ v * width + u ].blue  = f;

            f = sampler.cdfAt( u, v ) * pixel_max;
            f = std::min(f, (float)pixel_max);
            cdf_cache[ v * width + u ].red   = f;
            cdf_cache[ v * width + u ].green = f;
            cdf_cache[ v * width + u ].blue  = f;
        }
    }
#if 0
    // FIXME
    for( unsigned int v = 0; v < height; v++ ) {
        f = sampler.cdfAt( v ) * pixel_max;
        f = std::min(f, (float)pixel_max);
        cdf_cache[ v * (width+1) + width + 1 ].red   = f;
        cdf_cache[ v * (width+1) + width + 1 ].green = f;
        cdf_cache[ v * (width+1) + width + 1 ].blue  = f;
    }
#endif

    pdf_image->write( output_path + "/sample_pdf_2d.png" );
    cdf_image->write( output_path + "/sample_cdf_2d.png" );

#if 1
    // sample the pdf
    printf("Sampling PDF\n");
    std::vector<float> sample_dist(width * height);
    //for( int i = 0; i < 1000000; i++ ) {
    for( int i = 0; i < 100000; i++ ) {
    //for( int i = 0; i < 1000; i++ ) {
    //for( int i = 0; i < 100; i++ ) {
        DistributionSampler2D::Sample sample = sampler.sample();
        float u = sample.u * (width - 1);
        float v = sample.v * (height - 1);
        unsigned int index = (unsigned int) ((float)v * width + u);
        //printf("%03d %f %f %u  %f\n", i, u, v, index, sample.pdf);
        sample_dist[index] += 1.0f;
    }

    // normalize the sample distribution
    float maxsample = *std::max_element(sample_dist.begin(), sample_dist.end());
    printf("maxsample=%f\n", maxsample);

    // draw the sample distribution
    std::unique_ptr<Magick::Image> draws_image( new Magick::Image( Magick::Geometry( width, height ), "black" ) );
    draws_image->magick( "png" ); // set the output file type
    Magick::PixelPacket * draws_cache = draws_image->setPixels( 0, 0, width, height );
    for( unsigned int u = 0; u < width; u++ ) {
        for( unsigned int v = 0; v < height; v++ ) {
            unsigned int index = (unsigned int) v * width + u;
            f = sample_dist[index] / maxsample * pixel_max;
            f *= 1000.0f; // TEMP
            f = std::min(f, (float)pixel_max);
            draws_cache[ index ].red   = f;
            draws_cache[ index ].green = f;
            draws_cache[ index ].blue  = f;
        }
    }
    draws_image->write( output_path + "/sample_pdf_draws_2d.png" );
#endif
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
    testSamplePDF2D();
    
    total_run_timer.stop();
    printf("Done - Run time = %f seconds\n", total_run_timer.elapsed());
    fflush(stdout);
    return 0;
}

