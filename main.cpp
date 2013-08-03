#include <iostream>
#include <fstream>
#include <stdio.h>

// TEMP >>>
#include <Magick++.h>
// TEMP <<<

#include "Matrix.h"
#include "Ray.h"
#include "Sphere.h"
#include "RandomNumberGenerator.h"
#include "Scene.h"
#include "FlatContainer.h"
#include "AxisAlignedSlab.h"

RandomNumberGenerator rng;

void testMatrix() {
	Matrix4x4 A;
	A.identity();
	A.print();
	
	A.at( 1, 2 ) = 4.5;
	A.print();
    
	Matrix4x4 B( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 );
	B.print();
	
	Matrix4x4 C;
	
	mult( A, B, C );
	C.print();
}

void testSphere() {
	Sphere sphere( Vector4( 0.0, 0.0, 0.0 ), 5.0 );
	Ray ray( Vector4( 0.0, 0.0, -5.0 ), Vector4( 0.0, 0.0, 1.0 ) );
	RayIntersection intersection;
    
	bool hit = sphere.intersect( ray, intersection );
	
	printf( "Sphere hit=%s\n", hit ? "HIT" : "MISS" );
	intersection.position.print();
	intersection.normal.print();
}

void testRandom() {
    FILE * points_file = fopen( "/Users/dacunni/Projects/FastRender/output/rng.txt", "w" );
    float x, y, z;
    for( int i = 0; i < 1000; i++ ) {
        rng.uniformSurfaceUnitSphere( x, y, z );
        fprintf( points_file, "%f,%f,%f\n", x, y, z );
    }
    fclose( points_file );
}

void testManySpheres() {
    Sphere sphere( Vector4( 0.0, 0.0, 0.0 ), 5.0 );
	Ray ray( Vector4( 0.0, 0.0, 3.0 ), Vector4( 0.0, 0.0, -1.0 ) );
	RayIntersection intersection;
    
    int imageWidth = 500, imageHeight = 500;
    
    Matrix4x4 projectionMatrix;
    projectionMatrix.identity();
    
    
	Scene scene;
	FlatContainer * container = new FlatContainer();
	
    const int numSpheres = 20;
    
	for( int si = 0; si < numSpheres; si++ ) {
		container->add( new Sphere( Vector4( rng.uniformRange( -1.5, 1.5 ),
                                             rng.uniformRange( -1.5, 1.5 ),
                                             rng.uniformRange( -10.0, -5.0 ) ),
 								    0.15 ) );
	}
    
    container->add( new AxisAlignedSlab( -0.1-0.45, -0.1, -1.0,
                                          0.1-0.45,  0.1, -5.3 ) );
    container->add( new AxisAlignedSlab( -0.1+0.45, -0.1, -1.0,
                                          0.1+0.45,  0.1, -5.3 ) );
    container->add( new AxisAlignedSlab( -0.1, -0.1+0.45, -1.0,
                                          0.1,  0.1+0.45, -5.3 ) );
    container->add( new AxisAlignedSlab( -0.1, -0.1-0.45, -1.0,
                                          0.1,  0.1-0.45, -5.3 ) );

    container->add( new AxisAlignedSlab( -0.1-0.45, -0.1-0.45, -1.0,
                                          0.1-0.45,  0.1-0.45, -5.3 ) );
    container->add( new AxisAlignedSlab( -0.1-0.45, -0.1+0.45, -1.0,
                                          0.1-0.45,  0.1+0.45, -5.3 ) );
    container->add( new AxisAlignedSlab( -0.1+0.45, -0.1-0.45, -1.0,
                                          0.1+0.45,  0.1-0.45, -5.3 ) );
    container->add( new AxisAlignedSlab( -0.1+0.45, -0.1+0.45, -1.0,
                                          0.1+0.45,  0.1+0.45, -5.3 ) );

	scene.root = container;
    
    Magick::Image image( Magick::Geometry( imageWidth, imageHeight ), "white" );
    image.magick( "png" ); // set the output file type
    Magick::Image normal_image( Magick::Geometry( imageWidth, imageHeight ), "black" );
    normal_image.magick( "png" ); // set the output file type

    // FIXME - better handling of output files
    FILE * intersections_file = fopen( "/Users/dacunni/Projects/FastRender/output/intersections.txt", "w" );
    // write eye location
    ray.origin.fprintCSV(intersections_file);
    // intersect with scene
    float xmin = -0.15;
    float xmax = 0.15;
    float ymin = -0.15;
    float ymax = 0.15;

    for( int row = 0; row < imageHeight; row++ ) {
        for( int col = 0; col < imageWidth; col++ ) {
            ray.direction[0] = (float) col / imageWidth * (xmax - xmin) + xmin;
            ray.direction[1] = (float) row / imageHeight * (ymax - ymin) + ymin;
            ray.direction[2] = -1.0;
            ray.direction.normalize();
            bool hit = scene.intersect( ray, intersection );
            if( hit ) {
                intersection.position.fprintCSV( intersections_file );
                image.pixelColor(col, row, Magick::Color("black"));
                normal_image.pixelColor(col, row,
                                        Magick::ColorRGB(intersection.normal.x() * 0.5 + 0.5,
                                                         intersection.normal.y() * 0.5 + 0.5,
                                                         intersection.normal.z() * 0.5 + 0.5
                                                         ));
            }
        }
    }
    fclose( intersections_file );
	
    //image.pixelColor( 100, 100, Magick::Color("black") );
    image.write( "/Users/dacunni/Projects/FastRender/output/framebuffer.png" );
    normal_image.write( "/Users/dacunni/Projects/FastRender/output/normals.png" );
}

// TODO - make tests for Transforms

int main (int argc, char * const argv[]) {
    printf("FastRender\n"); fflush(stdout);

    rng.seedCurrentTime();

    //testMatrix();
    //testSphere();
    //testRandom();
    testManySpheres();
    
    printf("Done\n"); fflush(stdout);
    return 0;
}

