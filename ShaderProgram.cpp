#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "ShaderProgram.h"


const char * shaderTypeAsString( GLuint type )
{
    switch( type ) {
        case GL_VERTEX_SHADER:
            return "VERTEX SHADER";
        case GL_FRAGMENT_SHADER:
            return "FRAGMENT SHADER";
        default:
            return "UNKNOWN SHADER";
    }
}

void ShaderStage::loadFile( GLuint type, const std::string filename )
{
    printf("Loading shader from file %s\n", filename.c_str());
    std::ifstream ifs( filename );
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string src = ss.str();
    loadSource( type, src );
}

void ShaderStage::loadSource( GLuint type, const std::string & src )
{
    const char * type_string = shaderTypeAsString( type );
    int status = 0;

    id = glCreateShader( type );    
#if 0
    printf( ">>>> %s id=%u >>>>\n%s<<<< %s id=%u <<<<\n", 
            type_string, id, src.c_str(), type_string, id );
#endif

    // Make an array of pointers for GL
    const char * srcs[] = { src.c_str() };
    glShaderSource( id, 1, srcs, NULL );

    // Compile shader
    glCompileShader( id );
    glGetShaderiv( id, GL_COMPILE_STATUS, &status );

    printf( "ShaderStage Compile status: %s\n", status ? "OK" : "ERROR" );

    if( !status ) {
        GLint len = 0;
        glGetShaderiv( id, GL_INFO_LOG_LENGTH, &len );
        std::vector<GLchar> log( len );
        glGetShaderInfoLog( id, len, &len, &log[0] );
        printf( "Compiler Error Message:\n%s", (char *) &log[0] );
        glDeleteShader( id );
        id = 0;
    }
}

void ShaderProgram::create()
{
    id = glCreateProgram();
}

void ShaderProgram::attach( ShaderStage & shader )
{
    if( shader.id != 0 ) {
        glAttachShader( id, shader.id );
    }
}

void ShaderProgram::link()
{
    GLint status = 0;
    glLinkProgram( id );
    glGetProgramiv( id, GL_LINK_STATUS, &status ); 
    printf( "Link status: %s\n", status ? "OK" : "ERROR" );

    if( !status ) {
        GLint len = 0;
        glGetProgramiv( id, GL_INFO_LOG_LENGTH, &len );
        std::vector<GLchar> log( len );
        glGetProgramInfoLog( id, len, &len, &log[0] );
        printf( "Linker Error Message:\n%s", (char *) &log[0] );
        glDeleteProgram( id );
        id = 0;
    }
}

void ShaderProgram::use()
{
    glUseProgram(id);
    GL_WARN_IF_ERROR();
}

GLint ShaderProgram::attribLocation( const std::string & name )
{
    GLint loc = glGetAttribLocation(id, name.c_str());
    std::cout << "ATTRIB " << name << " @ " << loc << std::endl;
    return loc;
}

void ShaderProgram::fromVertexFragment( ShaderStage & vertexShader,
                                        ShaderStage & fragmentShader )
{
    create();
    attach(vertexShader);
    attach(fragmentShader);
    link();
}

void ShaderProgram::loadSourceVertexFragment( const std::string & vertexSource,
                                              const std::string & fragmentSource )
{
    ShaderStage vertexShader, fragmentShader;

    vertexShader.loadSource(GL_VERTEX_SHADER, vertexSource);
    if(!vertexShader.id)
        return;

    fragmentShader.loadSource(GL_FRAGMENT_SHADER, fragmentSource);
    if(!fragmentShader.id)
        return;

    fromVertexFragment(vertexShader, fragmentShader);
}

void ShaderProgram::loadFilesVertexFragment( const std::string & vertexFileName,
                                       const std::string & fragmentFileName )
{
    ShaderStage vertexShader, fragmentShader;

    vertexShader.loadFile(GL_VERTEX_SHADER, vertexFileName);
    if(!vertexShader.id)
        return;

    fragmentShader.loadFile(GL_FRAGMENT_SHADER, fragmentFileName);
    if(!fragmentShader.id)
        return;

    fromVertexFragment(vertexShader, fragmentShader);
}

