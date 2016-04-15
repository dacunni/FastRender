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
            printf("GPUMesh::upload\n");
            // Create vertex array object to hold everything
            glGenVertexArrays( 1, &vao );
            glBindVertexArray( vao );

            // Upload vertex positions
            glGenBuffers( 1, &vbo );
            glBindBuffer( GL_ARRAY_BUFFER, vbo );
            GLsizeiptr vsize = mesh.vertices.size() * sizeof(float) * 4;
            GLsizeiptr nsize = mesh.normals.size() * sizeof(float) * 4;
            // allocate some space for all of our attributes
            glBufferData( GL_ARRAY_BUFFER, vsize + nsize, NULL, GL_STATIC_DRAW );
            // upload positions
            glBufferSubData( GL_ARRAY_BUFFER, 0, vsize, &mesh.vertices[0].x );
            GL_WARN_IF_ERROR();
            glVertexAttribPointer( POSITION_ATTRIB_INDEX, 4, GL_FLOAT, GL_FALSE, 0, 0 );
            glEnableVertexAttribArray( POSITION_ATTRIB_INDEX );
            // upload normals
            glBufferSubData( GL_ARRAY_BUFFER, vsize, nsize, &mesh.normals[0].x );
            glVertexAttribPointer( NORMAL_ATTRIB_INDEX, 4, GL_FLOAT, GL_FALSE, 0, (void *) vsize );
            glEnableVertexAttribArray( NORMAL_ATTRIB_INDEX );
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

        const GLuint POSITION_ATTRIB_INDEX = 0;
        const GLuint NORMAL_ATTRIB_INDEX   = 1;

        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ibo = 0;
        unsigned long long num_vertices = 0;
};






#endif
