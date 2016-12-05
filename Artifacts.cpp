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

Artifacts::Artifacts( unsigned int imageWidth, unsigned int imageHeight )
: output_path( "output" ), // TEMP
    width( imageWidth ),
    height( imageHeight ),
    frame_number( 1 )
{
    image.reset( new Magick::Image( Magick::Geometry( imageWidth, imageHeight ), "black" ) );
    image->magick( "png" ); // set the output file type
    normal_image.reset( new Magick::Image( Magick::Geometry( imageWidth, imageHeight ), "black" ) );
    normal_image->magick( "png" ); // set the output file type
    depth_image.reset( new Magick::Image( Magick::Geometry( imageWidth, imageHeight ), Magick::ColorRGB(0.0f, 0.0f, 0.0f) ) );
    depth_image->magick( "png" ); // set the output file type
    time_image.reset( new Magick::Image( Magick::Geometry( imageWidth, imageHeight ), "black" ) );
    time_image->magick( "png" ); // set the output file type

    pixel_color_accum.assign( imageWidth * imageHeight, float3(0.0, 0.0, 0.0) );
    pixel_color_num_samples.assign( imageWidth * imageHeight, 0 );
    pixel_normal.assign( imageWidth * imageHeight, float3(0.0, 0.0, 0.0) );
    pixel_depth.assign( imageWidth * imageHeight, 0.0f );
    time_unnormalized_image.assign( imageWidth * imageHeight, 0.0 );

    intersections_file = fopen( (output_path + file_prefix + "/intersections.txt").c_str(), "w" );
}

Artifacts::~Artifacts()
{
    if( intersections_file )
        fclose( intersections_file );
}

// TODO - clean this up
void Artifacts::startNewFrame() 
{
    flush();

    image.reset( new Magick::Image( Magick::Geometry( width, height ), "black" ) );
    image->magick( "png" ); // set the output file type
    normal_image.reset( new Magick::Image( Magick::Geometry( width, height ), "black" ) );
    normal_image->magick( "png" ); // set the output file type
    depth_image.reset( new Magick::Image( Magick::Geometry( width, height ), Magick::ColorRGB(0.0f, 0.0f, 0.0f) ) );
    depth_image->magick( "png" ); // set the output file type
    time_image.reset( new Magick::Image( Magick::Geometry( width, height ), "black" ) );
    time_image->magick( "png" ); // set the output file type
    pixel_color_accum.assign( pixel_color_accum.size(), float3(0.0, 0.0, 0.0) );
    pixel_color_num_samples.assign( pixel_color_num_samples.size(), 0 );
    pixel_normal.assign( pixel_normal.size(), float3(0.0, 0.0, 0.0) );
    pixel_depth.assign( pixel_depth.size(), 0.0f );
    time_unnormalized_image.assign( time_unnormalized_image.size(), 0.0 );
    frame_number++;
}

void Artifacts::flush()
{
    char sindex[32];
    sprintf( sindex, "%08u", frame_number );

    // Use ImageMagick pixel caches to quickly write pixel data
    Magick::PixelPacket * color_cache = image->setPixels( 0, 0, width, height );
    Magick::PixelPacket * normal_cache = normal_image->setPixels( 0, 0, width, height );
    Magick::PixelPacket * depth_cache = depth_image->setPixels( 0, 0, width, height );

    unsigned short pixel_max = (1 << MAGICKCORE_QUANTUM_DEPTH) - 1;

    for( int row = 0; row < height; row++ ) {
        for( int col = 0; col < width; col++ ) {
            auto color = pixel_color_accum[ row * width + col ];
            auto nsamples = pixel_color_num_samples[ row * width + col ];
            if( nsamples > 0 ) {
                color.r /= nsamples;
                color.g /= nsamples;
                color.b /= nsamples;
            }
            color.r = std::min( color.r, 1.0f );
            color.g = std::min( color.g, 1.0f );
            color.b = std::min( color.b, 1.0f );
            color_cache[ row * width + col ].red   = color.r * pixel_max;
            color_cache[ row * width + col ].green = color.g * pixel_max;
            color_cache[ row * width + col ].blue  = color.b * pixel_max;

            auto normal = pixel_normal[ row * width + col ];
            normal_cache[ row * width + col ].red   = normal.x * pixel_max;
            normal_cache[ row * width + col ].green = normal.y * pixel_max;
            normal_cache[ row * width + col ].blue  = normal.z * pixel_max;

            auto depth = pixel_depth[ row * width + col ];
            depth_cache[ row * width + col ].red   = depth * pixel_max;
            depth_cache[ row * width + col ].green = depth * pixel_max;
            depth_cache[ row * width + col ].blue  = depth * pixel_max;
        }
    }

    image->write( output_path + "/" + file_prefix + "framebuffer_" + sindex + ".png" );
    normal_image->write( output_path + "/" + file_prefix + "normals.png" );
    depth_image->write( output_path + "/" + file_prefix + "depth.png" );

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
        value = std::max( value, 0.0 );
        value = std::min( value, 1.0 );
        time_cache[ i ].red   = value * pixel_max;
        time_cache[ i ].green = value * pixel_max;
        time_cache[ i ].blue  = value * pixel_max;
    }

    time_image->write( output_path + "/" + file_prefix + "time.png" );
    printf( "Average pixel render time: %f sec max: %f sec\n", average_value, max_value );
}

void Artifacts::accumPixelColorMono( unsigned int row, unsigned int col, float value )
{
    pixel_color_accum[ row * width + col ][0] += value;
    pixel_color_accum[ row * width + col ][1] += value;
    pixel_color_accum[ row * width + col ][2] += value;
    pixel_color_num_samples[ row * width + col ]++;
}

void Artifacts::accumPixelColorRGB( unsigned int row, unsigned int col, float r, float g, float b )
{
    pixel_color_accum[ row * width + col ][0] += r;
    pixel_color_accum[ row * width + col ][1] += g;
    pixel_color_accum[ row * width + col ][2] += b;
    pixel_color_num_samples[ row * width + col ]++;
}

void Artifacts::setPixelNormal( unsigned int row, unsigned int col, const Vector4 & n )
{
    pixel_normal[ row * width + col ][0] = n.x * 0.5 + 0.5;
    pixel_normal[ row * width + col ][1] = n.y * 0.5 + 0.5;
    pixel_normal[ row * width + col ][2] = n.z * 0.5 + 0.5;
}

void Artifacts::setPixelDepth( unsigned int row, unsigned int col, float depth )
{
    depth = (1.0 - (depth - 3.0) / 20.0);
    depth = std::min( std::max( depth, 0.0f ), 1.0f );

    pixel_depth[ row * width + col ] = depth;
}

void Artifacts::accumPixelTime( unsigned int row, unsigned int col, float value )
{
    time_unnormalized_image[ row * width + col ] += value;
}

void Artifacts::setPixelTime( unsigned int row, unsigned int col, float value )
{
    time_unnormalized_image[ row * width + col ] = value;
}


