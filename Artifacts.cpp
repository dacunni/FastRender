//
//  Artifacts.cpp
//  FastRender
//
//  Created by David Cunningham on 8/23/13.
//
//

#include <stdio.h>
#include <algorithm>
#include "Vector.h"
#include "Artifacts.h"

Artifacts::Artifacts( unsigned int imageWidth, unsigned int imageHeight )
: output_path( "output" ), // TEMP
    width( imageWidth ),
    height( imageHeight )
{
    image = new Magick::Image( Magick::Geometry( imageWidth, imageHeight ), "black" );
    image->magick( "png" ); // set the output file type
    normal_image = new Magick::Image( Magick::Geometry( imageWidth, imageHeight ), "black" );
    normal_image->magick( "png" ); // set the output file type
    depth_image = new Magick::Image( Magick::Geometry( imageWidth, imageHeight ), Magick::ColorRGB(0.0f, 0.0f, 0.0f) );
    depth_image->magick( "png" ); // set the output file type
    time_image = new Magick::Image( Magick::Geometry( imageWidth, imageHeight ), "black" );
    time_image->magick( "png" ); // set the output file type
    time_unnormalized_image.resize( imageWidth * imageHeight );
    intersections_file = fopen( (output_path + "/intersections.txt").c_str(), "w" );

}

Artifacts::~Artifacts()
{
    if( image )
        delete image;
    if( depth_image )
        delete depth_image;
    if( normal_image )
        delete normal_image;
    if( time_image )
        delete time_image;
    if( intersections_file )
        fclose( intersections_file );
}

void Artifacts::flush()
{
    image->write( output_path + "/framebuffer.png" );
    normal_image->write( output_path + "/normals.png" );
    depth_image->write( output_path + "/depth.png" );

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
    for( int i = 0; i < width * height; i++ ) {
        double value = (time_unnormalized_image[i] - min_value) / max_value;
        value = std::max( value, 0.0 );
        value = std::min( value, 1.0 );
        time_image->pixelColor( i % width, i / width, Magick::ColorRGB( value, value, value ) );
    }

    time_image->write( output_path + "/time.png" );
    printf( "Average pixel render time: %f sec max: %f sec\n", average_value, max_value );
}

void Artifacts::setPixelColorMono( unsigned int row, unsigned int col, float value )
{
    image->pixelColor( col, row, Magick::ColorRGB( value, value, value ) );
}

void Artifacts::setPixelNormal( unsigned int row, unsigned int col, const Vector4 & n )
{
    normal_image->pixelColor( col, row,
                              Magick::ColorRGB( n.x * 0.5 + 0.5,
                                                n.y * 0.5 + 0.5,
                                                n.z * 0.5 + 0.5 ) );
}

void Artifacts::setPixelDepth( unsigned int row, unsigned int col, float depth )
{
    depth = (1.0 - (depth - 3.0) / 10.0);
    depth = std::min( std::max( depth, 0.0f ), 1.0f );

    depth_image->pixelColor( col, row, Magick::ColorRGB( depth, depth, depth ) );
}

void Artifacts::setPixelTime( unsigned int row, unsigned int col, float value )
{
    time_unnormalized_image[ row * width + col ] = value;
    //time_image->pixelColor( col, row, Magick::ColorRGB( value, value, value ) );
}


