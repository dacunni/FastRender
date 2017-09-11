#include <iostream>
#include "EditorSceneGraph.h"
#include "Scene.h"
#include "Traceable.h"
#include "TraceableVisitor.h"
#include "FlatContainer.h"
#include "Sphere.h"
#include "AxisAlignedSlab.h"
#include "TriangleMesh.h"
#include "ShaderProgram.h"
#include "SimpleCamera.h"

ObjectEditor::ObjectEditor()
{
}

ObjectEditor::~ObjectEditor()
{
    glDeleteVertexArrays(1, &vertexArray);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
}
std::string ObjectEditor::label() { return "Object"; }
void ObjectEditor::draw(SimpleCamera & camera, ShaderProgram & shaderProgram)
{
    printf("VAO %u VBO %u IBO %u numVertices %u numIndices %u\n", vertexArray, vertexBuffer, indexBuffer, numVertices, numIndices);

    GLuint worldLoc = shaderProgram.uniformLocation("world");
    GLuint viewLoc = shaderProgram.uniformLocation("view");
    GLuint projectionLoc = shaderProgram.uniformLocation("projection");

    Matrix4x4 world;
    Matrix4x4 view;
    Matrix4x4 projection;

    world.identity();
    if(object().transform)
        world = object().transform->fwd;
    view.identity();
    view = camera.transform.rev;
    projection.glProjectionSymmetric(0.2, 0.2, 0.25, 200.0); // TODO - real camera projection

    printf("WORLD TRANSFORM:\n"); world.print();
    printf("VIEW:\n"); view.print();
    printf("PROJECTION:\n"); projection.print();

    glUniformMatrix4fv(worldLoc, 1, GL_TRUE, world.data);
    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, view.data);
    glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, projection.data);

    if(vertexArray) {
        glBindVertexArray(vertexArray);
        GL_WARN_IF_ERROR();

        if(indexBuffer) {
            glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, NULL);
        }
        else {
            glDrawArrays(GL_TRIANGLES, 0, numVertices);
        }
        GL_WARN_IF_ERROR();
        glBindVertexArray(0);
    }
}

void ObjectEditor::buildGpuBuffers(ShaderProgram & shaderProgram) {
    std::cout << "TODO buildGpuBuffers for " << label() << std::endl;
}


class SphereEditor : public ObjectEditor {
    public:
        SphereEditor(Sphere & o) : obj(o) {}
        ~SphereEditor() {}

        virtual std::string label() { return "Sphere"; }
        virtual Traceable & object() const { return obj; }

        virtual void buildGpuBuffers(ShaderProgram & shaderProgram)
        {
            glGenVertexArrays(1, &vertexArray);
            glGenBuffers(1, &vertexBuffer);
            //glGenBuffers(1, &indexBuffer);
            glBindVertexArray(vertexArray);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

            struct Vertex { float x, y, z; };

            unsigned int numSlices = 10;
            unsigned int numSteps = 20;
            std::vector<Vertex> vertices;
            vertices.reserve(numSlices * numSteps);

            Vertex center = { .x = obj.center.x, .y = obj.center.y, .z = obj.center.z };
            float radius = obj.radius;
            float dlat = (float) 1.0 / numSlices * M_PI;
            float dlon = (float) 1.0 / numSteps * 2.0 * M_PI;
            float lat = -M_PI * 0.5;
            for(unsigned int slice = 0; slice < numSlices; slice++, lat += dlat) {
                float lon = 0.0;
                for(unsigned int step = 0; step < numSteps; step++, lon += dlon) {
                    vertices.push_back( { .x = center.x + radius * cosf(lat) * cosf(lon),
                                          .y = center.y + radius * cosf(lat) * sinf(lon),
                                          .z = center.z + radius * sinf(lat) } );
                    vertices.push_back( { .x = center.x + radius * cosf(lat) * cosf(lon + dlon),
                                          .y = center.y + radius * cosf(lat) * sinf(lon + dlon),
                                          .z = center.z + radius * sinf(lat) } );
                    vertices.push_back( { .x = center.x + radius * cosf(lat + dlat) * cosf(lon),
                                          .y = center.y + radius * cosf(lat + dlat) * sinf(lon),
                                          .z = center.z + radius * sinf(lat + dlat) } );

                    vertices.push_back( { .x = center.x + radius * cosf(lat + dlat) * cosf(lon),
                                          .y = center.y + radius * cosf(lat + dlat) * sinf(lon),
                                          .z = center.z + radius * sinf(lat + dlat) } );
                    vertices.push_back( { .x = center.x + radius * cosf(lat + dlat) * cosf(lon),
                                          .y = center.y + radius * cosf(lat + dlat) * sinf(lon),
                                          .z = center.z + radius * sinf(lat + dlat) } );
                    vertices.push_back( { .x = center.x + radius * cosf(lat + dlat) * cosf(lon + dlon),
                                          .y = center.y + radius * cosf(lat + dlat) * sinf(lon + dlon),
                                          .z = center.z + radius * sinf(lat + dlat) } );
                }
            }
            numVertices = vertices.size();

            auto positionLoc = shaderProgram.attribLocation("position");

            glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
            glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
            glEnableVertexAttribArray(positionLoc);
            GL_WARN_IF_ERROR();

            glBindVertexArray(0);
        }

        Sphere & obj;
};

class FlatContainerEditor : public ObjectEditor {
    public:
        FlatContainerEditor(FlatContainer & o) : obj(o) {}
        ~FlatContainerEditor() {}

        virtual std::string label() { return "Flat Container"; }
        virtual Traceable & object() const { return obj; }

        FlatContainer & obj;
};

class TriangleMeshEditor : public ObjectEditor {
    public:
        TriangleMeshEditor(TriangleMesh & o) : obj(o) {}
        ~TriangleMeshEditor() {}

        virtual std::string label() { return "Triangle Mesh"; }
        virtual Traceable & object() const { return obj; }

        TriangleMesh & obj;
};

class AxisAlignedSlabEditor : public ObjectEditor {
    public:
        AxisAlignedSlabEditor(AxisAlignedSlab & o) : obj(o) {}
        ~AxisAlignedSlabEditor() {}

        virtual std::string label() { return "Box"; }
        virtual Traceable & object() const { return obj; }

        AxisAlignedSlab & obj;
};

void EditorSceneGraphNode::print(unsigned int depth) {
    std::cout << std::string(depth, '\t') << "EditorSceneGraphNode (" << editor->label() << ")" << std::endl;
    for(auto & child : children) {
        child->print(depth + 1);
    }
}

void EditorSceneGraphNode::buildGpuBuffers(ShaderProgram & shaderProgram) {
    editor->buildGpuBuffers(shaderProgram);
    for(auto & child : children) {
        child->buildGpuBuffers(shaderProgram);
    }
}

void EditorSceneGraphNode::draw(SimpleCamera & camera, ShaderProgram & shaderProgram) {
    editor->draw(camera, shaderProgram);
    for(auto & child : children) {
        child->draw(camera, shaderProgram);
    }
}

class EditorSceneGraphBuilder : public TraceableVisitor {
    public:
        EditorSceneGraphBuilder(EditorSceneGraph & sg) : sceneGraph(sg) {}
        ~EditorSceneGraphBuilder() {}

        virtual void walk( Traceable & t ) {
            TraceableVisitor::walk(t);
        }

        virtual void handle( FlatContainer & t ) {
            //std::cout << std::string(buildStack.size(), '\t') << "building FlatContainer" << std::endl;
            EditorSceneGraphNode * node = new EditorSceneGraphNode(new FlatContainerEditor(t));
            if(!sceneGraph.root) { sceneGraph.root = node; }
            else { add(node); }
            buildStack.push_back(node);
            for( auto & child : t.objects ) {
                walk(*child);
            }
            buildStack.pop_back();
        }
        virtual void handle( Sphere & t ) {
            //std::cout << std::string(buildStack.size(), '\t') << "building Sphere" << std::endl;
            EditorSceneGraphNode * node = new EditorSceneGraphNode(new SphereEditor(t));
            add(node);
        }
        virtual void handle( AxisAlignedSlab & t ) {
            //std::cout << std::string(buildStack.size(), '\t') << "building AxisAlignedSlab" << std::endl;
            EditorSceneGraphNode * node = new EditorSceneGraphNode(new AxisAlignedSlabEditor(t));
            add(node);
        }
        virtual void handle( TriangleMesh & t ) {
            //std::cout << std::string(buildStack.size(), '\t') << "building TriangleMesh" << std::endl;
            EditorSceneGraphNode * node = new EditorSceneGraphNode(new TriangleMeshEditor(t));
            add(node);
        }

        void add(EditorSceneGraphNode * node) {
            buildStack.back()->children.push_back(node);
        }

        EditorSceneGraph & sceneGraph;
        std::vector<EditorSceneGraphNode *> buildStack;
};

EditorSceneGraph::EditorSceneGraph()
    : root(nullptr)
{

}

EditorSceneGraph::~EditorSceneGraph()
{

}

void EditorSceneGraph::build( Scene & scene )
{
    std::cout << "SCENE" << std::endl; // TEMP
    build(*scene.root);
}

void EditorSceneGraph::build( Traceable & traceable )
{
    std::cout << "TRACEABLE" << std::endl; // TEMP
    EditorSceneGraphBuilder builder(*this);
    builder.walk(traceable);
}

void EditorSceneGraph::print()
{
    if(root) {
        root->print();
    }
}

void EditorSceneGraph::buildGpuBuffers(ShaderProgram & shaderProgram)
{
    if(root) {
        root->buildGpuBuffers(shaderProgram);
    }
}

void EditorSceneGraph::draw(SimpleCamera & camera, ShaderProgram & shaderProgram)
{
    if(root) {
        root->draw(camera, shaderProgram);
    }
}


