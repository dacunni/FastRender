/*
 *  GPUMesh.h
 *  FastRender
 *
 *  Created by David Cunningham on 4/11/2014
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _GPU_MESH_H_
#define _GPU_MESH_H_

// TODO - make an implementation file for these methods

class TriangleMesh;

class GPUMesh
{
    public:
        GPUMesh() 
        {

        }

        ~GPUMesh() 
        {
            // IMPLEMENT ME - free gpu resources for VAO,etc
        }

        void upload( TriangleMesh & mesh )
        {
            // Create vertex array object to hold everything
            glGenVertexArrays( 1, &vao );
            glBindVertexArray( vao );

            // Upload vertex positions
            glGenBuffers( 1, &vbo );
            glBindBuffer( GL_ARRAY_BUFFER, vbo );
            glBufferData( GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float) * 4, &mesh.vertices[0].x, GL_STATIC_DRAW );
            glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0, 0 );
            glEnableVertexAttribArray( 0 );
            GL_WARN_IF_ERROR();

            // Upload vertex indices
            glGenBuffers( 1, &ibo );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, mesh.triangles.size() * sizeof(unsigned int) * 3, &mesh.triangles[0], GL_STATIC_DRAW );
            GL_WARN_IF_ERROR();

            num_vertices = mesh.triangles.size() * 3;
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
                glDrawArrays( GL_TRIANGLES, 0, num_vertices );
                GL_WARN_IF_ERROR();
            }
        }

        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ibo = 0;
        unsigned long long num_vertices = 0;
};






#endif
