//
//  Artifacts.cpp
//  FastRender
//
//  Created by David Cunningham on 8/23/13.
//
//

#include <stdio.h>
#include <iostream>
#include <memory>
#include <algorithm>
#include "Vector.h"
#include "Artifacts.h"
#include "GeometryUtils.h"

Artifacts::Artifacts( unsigned int imageWidth, unsigned int imageHeight )
: output_path( "output" ), // TEMP
    width( imageWidth ),
    height( imageHeight ),
    frame_number( 0 ),
    preview_window( *this )
{
    intersections_file = fopen( (output_path + file_prefix + "/intersections.txt").c_str(), "w" );
}

Artifacts::~Artifacts()
{
    if( intersections_file )
        fclose( intersections_file );
}

void Artifacts::startNewFrame() 
{
    printf("Starting new frame artifacts\n");
    printf("  output_path : %s\n", output_path.c_str());
    printf("  file_prefix : %s\n", file_prefix.c_str());
    if( frame_number > 0 )
        flush();
    resetImages();
    frame_number++;

    if( show_preview_window && frame_number == 1 ) {
        //preview_window.init();
    }
}

void Artifacts::resetImages()
{
    image.reset( new Magick::Image( Magick::Geometry( width, height ), "black" ) );
    image->magick( "png" ); // set the output file type
    normal_image.reset( new Magick::Image( Magick::Geometry( width, height ), "black" ) );
    normal_image->magick( "png" ); // set the output file type
    depth_image.reset( new Magick::Image( Magick::Geometry( width, height ), Magick::ColorRGB(0.0f, 0.0f, 0.0f) ) );
    depth_image->magick( "png" ); // set the output file type
    time_image.reset( new Magick::Image( Magick::Geometry( width, height ), "black" ) );
    time_image->magick( "png" ); // set the output file type
    stddev_image.reset( new Magick::Image( Magick::Geometry( width, height ), "black" ) );
    stddev_image->magick( "png" ); // set the output file type

    pixel_color_accum.assign( width * height, float3(0.0, 0.0, 0.0) );
    pixel_color_sq_accum.assign( width * height, float3(0.0, 0.0, 0.0) );
    pixel_color_num_samples.assign( width * height, 0 );
    pixel_normal.assign( width * height, float3(0.0, 0.0, 0.0) );
    pixel_depth.assign( width * height, 0.0f );
    time_unnormalized_image.assign( width * height, 0.0 );
}

// Scale, clamp, and assign a color to an ImageMagick pixel
void set( Magick::PixelPacket & packet, const float3 & color )
{
    const unsigned short pixel_max = (1 << MAGICKCORE_QUANTUM_DEPTH) - 1;
    packet.red   = clamp01(color.r) * pixel_max;
    packet.green = clamp01(color.g) * pixel_max;
    packet.blue  = clamp01(color.b) * pixel_max;
}

void Artifacts::flush()
{
    char sindex[32];
    sprintf( sindex, "%08u", frame_number );

    // Use ImageMagick pixel caches to quickly write pixel data
    Magick::PixelPacket * color_cache = image->setPixels( 0, 0, width, height );
    Magick::PixelPacket * normal_cache = normal_image->setPixels( 0, 0, width, height );
    Magick::PixelPacket * depth_cache = depth_image->setPixels( 0, 0, width, height );
    Magick::PixelPacket * stddev_cache = stddev_image->setPixels( 0, 0, width, height );

    // Largest representable channel value in the output images
    unsigned short pixel_max = (1 << MAGICKCORE_QUANTUM_DEPTH) - 1;

    for( int row = 0; row < height; row++ ) {
        for( int col = 0; col < width; col++ ) {
            auto pindex = row * width + col;
            auto color = pixel_color_accum[pindex];
            auto nsamples = pixel_color_num_samples[pindex];
            if( nsamples > 0 ) {
                color.r /= nsamples;
                color.g /= nsamples;
                color.b /= nsamples;
            }
            set( color_cache[pindex], color );

            auto color_sq = pixel_color_sq_accum[pindex];
            if( nsamples > 0 ) {
                color_sq.r /= nsamples;
                color_sq.g /= nsamples;
                color_sq.b /= nsamples;
            }
            auto stddev = float3(sqrtf(color_sq.r - sq(color.r)),
                                 sqrtf(color_sq.g - sq(color.g)),
                                 sqrtf(color_sq.b - sq(color.b)));
            //float scale = 1.0f; // arbitrary scale to brighten images as necessary
            //stddev.r *= scale;
            //stddev.g *= scale;
            //stddev.b *= scale;
            set( stddev_cache[pindex], stddev );

            set( normal_cache[pindex], pixel_normal[pindex] );

            auto depth = pixel_depth[pindex];
            set( depth_cache[pindex], float3(depth, depth, depth) );
        }
    }

    image->write( output_path + "/" + file_prefix + "framebuffer_" + sindex + ".png" );
    normal_image->write( output_path + "/" + file_prefix + "normals.png" );
    depth_image->write( output_path + "/" + file_prefix + "depth.png" );
    stddev_image->write( output_path + "/" + file_prefix + "stddev.png" );

    // Find the maximum time taken to render a pixel
    double min_value = 0.0, max_value = 0.0;
    double average_value = 0.0;
    for( int i = 0; i < width * height; i++ ) {
        if( time_unnormalized_image[i] > max_value ) {
            max_value = time_unnormalized_image[i];
        }
        average_value += time_unnormalized_image[i];
    }

    average_value /= (width * height);

    // Create sorted vector of times so we can do a value stretch that is less
    // sensitive to outliers than a simple min/max stretch
    std::vector<double> sorted_times( time_unnormalized_image );
    std::sort( sorted_times.begin(), sorted_times.end() );
    double percentile = 0.01;
    min_value = sorted_times[ percentile * width * height - 1 ];
    max_value = sorted_times[  (1.0 - percentile) * width * height - 1 ];

    // Normalize the time image by the maximum time
    Magick::PixelPacket * time_cache = time_image->setPixels( 0, 0, width, height );
    for( int i = 0; i < width * height; i++ ) {
        double value = (time_unnormalized_image[i] - min_value) / max_value;
        set( time_cache[ i ], float3(value, value, value) );
    }

    time_image->write( output_path + "/" + file_prefix + "time.png" );
    //printf( "Average pixel render time: %f sec max: %f sec\n", average_value, max_value );
}

void Artifacts::accumPixelColorMono( unsigned int row, unsigned int col, float value )
{
    auto pindex = row * width + col;
    pixel_color_accum[pindex][0] += value;
    pixel_color_accum[pindex][1] += value;
    pixel_color_accum[pindex][2] += value;
    pixel_color_sq_accum[pindex][0] += value * value;
    pixel_color_sq_accum[pindex][1] += value * value;
    pixel_color_sq_accum[pindex][2] += value * value;
    pixel_color_num_samples[pindex]++;
}

void Artifacts::accumPixelColorRGB( unsigned int row, unsigned int col, float r, float g, float b )
{
    auto pindex = row * width + col;
    pixel_color_accum[pindex][0] += r;
    pixel_color_accum[pindex][1] += g;
    pixel_color_accum[pindex][2] += b;
    pixel_color_sq_accum[pindex][0] += r * r;
    pixel_color_sq_accum[pindex][1] += g * g;
    pixel_color_sq_accum[pindex][2] += b * b;
    pixel_color_num_samples[pindex]++;
}

void Artifacts::resetPixelColor( unsigned int row, unsigned int col )
{
    auto pindex = row * width + col;
    pixel_color_accum[pindex][0] = 0.0f;
    pixel_color_accum[pindex][1] = 0.0f;
    pixel_color_accum[pindex][2] = 0.0f;
    pixel_color_sq_accum[pindex][0] = 0.0f;
    pixel_color_sq_accum[pindex][1] = 0.0f;
    pixel_color_sq_accum[pindex][2] = 0.0f;
    pixel_color_num_samples[pindex] = 0;
}

void Artifacts::setPixelNormal( unsigned int row, unsigned int col, const Vector4 & n )
{
    auto pindex = row * width + col;
    pixel_normal[pindex][0] = n.x * 0.5 + 0.5;
    pixel_normal[pindex][1] = n.y * 0.5 + 0.5;
    pixel_normal[pindex][2] = n.z * 0.5 + 0.5;
}

void Artifacts::setPixelDepth( unsigned int row, unsigned int col, float depth )
{
    auto pindex = row * width + col;

#if 1
    float near = 1.0f;
    float far = 20.0f;
    //depth = (2.0 * near) / (far + near - depth * (far - near));
    depth = (depth - near) / (far - near);
#elif 1
    depth = log2f(depth);
#else
    depth = (1.0 - (depth - 3.0) / 20.0);
#endif
    depth = std::min( std::max( depth, 0.0f ), 1.0f );

    pixel_depth[pindex] = depth;
}

void Artifacts::accumPixelTime( unsigned int row, unsigned int col, float value )
{
    auto pindex = row * width + col;
    time_unnormalized_image[pindex] += value;
}

void Artifacts::setPixelTime( unsigned int row, unsigned int col, float value )
{
    auto pindex = row * width + col;
    time_unnormalized_image[pindex] = value;
}

float Artifacts::pixelMaxChannelVariance( unsigned int row, unsigned int col )
{
    auto pindex = row * width + col;
    auto nsamples = pixel_color_num_samples[pindex];
    auto color = pixel_color_accum[pindex];
    auto color_sq = pixel_color_sq_accum[pindex];
    if( nsamples > 0 ) {
        color.r /= nsamples;
        color.g /= nsamples;
        color.b /= nsamples;
        color_sq.r /= nsamples;
        color_sq.g /= nsamples;
        color_sq.b /= nsamples;
    }
    return std::max(std::max(color_sq.r - sq(color.r),
                             color_sq.g - sq(color.g)),
                    color_sq.b - sq(color.b));
}

