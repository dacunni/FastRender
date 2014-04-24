/*
 *  GPUPointCloud.h
 *  FastRender
 *
 *  Created by David Cunningham on 4/11/2014
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _GPU_POINT_CLOUD_H_
#define _GPU_POINT_CLOUD_H_

#include "Vector.h"

// TODO - make an implementation file for these methods

class GPUPointCloud
{
    public:
        GPUPointCloud() 
        {

        }

        ~GPUPointCloud() 
        {
            // IMPLEMENT ME - free gpu resources for VAO,etc
        }

        void upload( std::vector<Vector4> & points )
        {
            printf("GPUPointCloud::upload\n");
            // Create vertex array object to hold everything
            glGenVertexArrays( 1, &vao );
            glBindVertexArray( vao );

            // Upload vertex positions
            glGenBuffers( 1, &vbo );
            glBindBuffer( GL_ARRAY_BUFFER, vbo );
            GLsizeiptr vsize = points.size() * sizeof(points[0]);
            // allocate some space for all of our attributes
            glBufferData( GL_ARRAY_BUFFER, vsize, NULL, GL_STATIC_DRAW );
            // upload positions
            glBufferSubData( GL_ARRAY_BUFFER, 0, vsize, &points[0] );
            GL_WARN_IF_ERROR();
            glVertexAttribPointer( POSITION_ATTRIB_INDEX, 4, GL_FLOAT, GL_FALSE, 0, 0 );
            glEnableVertexAttribArray( POSITION_ATTRIB_INDEX );

            num_vertices = points.size();
        }

        bool uploaded()
        {
            return vao != 0;
        }

        void bind()
        {
            if( vao ) {
                glBindVertexArray( vao );
                GL_WARN_IF_ERROR();
            }
        }

        void draw()
        {
            if( vao ) {
                glPointSize( point_size );
                glDrawArrays( GL_POINTS, 0, num_vertices );
                glPointSize( 1 );
                GL_WARN_IF_ERROR();
            }
        }

        void setPointSize( float s ) { point_size = s; }

        const GLuint POSITION_ATTRIB_INDEX = 0;
        const GLuint NORMAL_ATTRIB_INDEX   = 1;

        GLuint vao = 0;
        GLuint vbo = 0;
        unsigned long long num_vertices = 0;
        float point_size = 1;
};






#endif
