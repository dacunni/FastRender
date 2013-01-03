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
	Ray ray( Vector4( 0.0, 0.0, -5.0 ), Vector4( 0.0, 0.0, 1.0 ) );
	RayIntersection intersection;
        
	Scene scene;
	FlatContainer * container = new FlatContainer();
	
	for( int si = 0; si < 20; si++ ) {
		container->add( new Sphere( Vector4( rng.uniformRange( -1.5, 1.5 ),
                                            rng.uniformRange( -1.5, 1.5 ),
                                            rng.uniformRange( -0.5, 0.5 ) ),
								   0.15 ) );
	}
	
	scene.root = container;
    
    // FIXME - better handling of output files
    FILE * intersections_file = fopen( "/Users/dacunni/Projects/FastRender/output/intersections.txt", "w" );
    // write eye location
    ray.origin.fprintCSV(intersections_file);
    // intersect with scene
    int numSpheres = 20000;
    printf( "Intersecting with %d spheres\n", numSpheres );
	for( int i = 0; i < numSpheres; i++ ) {
		ray.direction[1] = rng.uniformRange( -0.25, 0.25 );
		ray.direction[0] = rng.uniformRange( -0.25, 0.25 );
		ray.direction.normalize();
		bool hit = scene.intersect( ray, intersection );
        if( hit ) {
            intersection.position.fprintCSV( intersections_file );
        }
    }
    fclose( intersections_file );
	
    Magick::Image image( "500x500", "white" );
    image.magick( "png" ); // set the output file type
    image.pixelColor( 100, 100, Magick::Color("red") );
    image.write( "/Users/dacunni/Projects/FastRender/output/framebuffer.png" );
}

int main (int argc, char * const argv[]) {
    printf("FastRender\n");

    rng.seedCurrentTime();

    //testMatrix();
    //testSphere();
    //testRandom();
    testManySpheres();
    
    return 0;
}

