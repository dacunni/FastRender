#ifndef _SHADER_PROGRAM_H_
#define _SHADER_PROGRAM_H_

#include <string>

#include "OpenGLUtil.h"

const char * shaderTypeAsString( GLuint type );

class ShaderStage {
    public:
        ShaderStage() : id(0) {}
        ~ShaderStage() {}

        void loadFile( GLuint type, const std::string filename );
        void loadSource( GLuint type, const std::string & src );

        GLuint id;
};

class ShaderProgram {
    public:
        ShaderProgram() : id(0) {}
        ~ShaderProgram() {}

        void create();
        void attach( ShaderStage & shader );
        void link();
        void use();

        void fromVertexFragment( ShaderStage & vertexShader,
                                 ShaderStage & fragmentShader );
        void loadSourceVertexFragment( const std::string & vertexSource,
                                       const std::string & fragmentSource );
        void loadFilesVertexFragment( const std::string & vertexFileName,
                                      const std::string & fragmentFileName );

        GLuint id;
};




#endif
