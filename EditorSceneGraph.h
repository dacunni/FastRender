#ifndef _EDITOR_SCENE_GRAPH_H_
#define _EDITOR_SCENE_GRAPH_H_

#include <vector>
#include "OpenGLUtil.h"

class Scene;
class Traceable;
class ShaderProgram;
class SimpleCamera;
class Matrix4x4;

class ObjectEditor {
    public:
        ObjectEditor();
        ~ObjectEditor();

        // Shader data structures
        struct Vertex {
            struct Position { float x, y, z; } position;
            struct Normal { float x, y, z; } normal;
        };

        virtual std::string label();
        virtual void draw(SimpleCamera & camera,
                          ShaderProgram & shaderProgram,
                          const Matrix4x4 * nodeWorldMatrix = nullptr);

        virtual void buildGpuBuffers(ShaderProgram & shaderProgram);

        virtual Traceable & object() const = 0;

        // OpenGL buffers
        GLuint vertexArray = 0;
        GLuint vertexBuffer = 0;
        GLuint indexBuffer = 0;
        unsigned int numVertices = 0;
        unsigned int numIndices = 0;
};

struct EditorSceneGraphNode {
    EditorSceneGraphNode(ObjectEditor * e) : editor(e) {}
    ~EditorSceneGraphNode() {}

    void print(unsigned int depth = 0);

    void buildGpuBuffers(ShaderProgram & shaderProgram);
    void draw(SimpleCamera & camera,
              ShaderProgram & shaderProgram,
              const Matrix4x4 * parentWorldMatrix = nullptr);

    ObjectEditor * editor;
    std::vector<EditorSceneGraphNode *> children;
};

class EditorSceneGraph {
public:
	EditorSceneGraph();
    virtual ~EditorSceneGraph();

    void build( Scene & scene );
    void build( Traceable & traceable );
    void print();
    void buildGpuBuffers(ShaderProgram & shaderProgram);
    void draw(SimpleCamera & camera, ShaderProgram & shaderProgram);

    EditorSceneGraphNode * root;
};

#endif
