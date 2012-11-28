#include <iostream>
#include <fstream>
#include <stdio.h>

#include "Matrix.h"
#include "Ray.h"
#include "Sphere.h"
#include "RandomNumberGenerator.h"
#include "Scene.h"
#include "FlatContainer.h"

int main (int argc, char * const argv[]) {
	
#if 0
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
#endif
	
	Sphere sphere( Vector4( 0.0, 0.0, 0.0 ), 5.0 );
	Ray ray( Vector4( 0.0, 0.0, -5.0 ), Vector4( 0.0, 0.0, 1.0 ) );
	RayIntersection intersection;

#if 0
	bool hit = sphere.intersect( ray, intersection );
	
	printf( "Sphere hit=%s\n", hit ? "HIT" : "MISS" );
	intersection.position.print();
	intersection.normal.print();
#endif

	RandomNumberGenerator rng;
	rng.seedCurrentTime();

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
	for( int i = 0; i < 2000; i++ ) {
		ray.direction[1] = rng.uniformRange( -0.25, 0.25 );
		ray.direction[0] = rng.uniformRange( -0.25, 0.25 );		
		ray.direction.normalize();
		bool hit = scene.intersect( ray, intersection );
        if( hit ) {
            intersection.position.fprintCSV( intersections_file );
        }
    }
    fclose( intersections_file );
	
#if 1
    FILE * points_file = fopen( "/Users/dacunni/Projects/FastRender/output/rng.txt", "w" );
    float x, y, z;
    for( int i = 0; i < 1000; i++ ) {
        rng.uniformSurfaceUnitSphere( x, y, z );
        fprintf( points_file, "%f,%f,%f\n", x, y, z );
    }
    fclose( points_file );
#endif
	
#if 0
	for( int i = 0; i < 1000000; i++ ) {
		bool hit = intersect( ray, sphere, intersection );
	}
#endif
	
    return 0;
}

