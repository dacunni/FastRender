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
    //printf("VAO %u VBO %u IBO %u numVertices %u numIndices %u\n", vertexArray, vertexBuffer, indexBuffer, numVertices, numIndices);

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

    float xmin, xmax, ymin, ymax;
    camera.getFocalPlaneDimensions(xmin, xmax, ymin, ymax);
    projection.glProjection(xmin, xmax, ymin, ymax, 1.0, 1000.0);

    //printf("WORLD TRANSFORM:\n"); world.print();
    //printf("VIEW:\n"); view.print();
    //printf("PROJECTION:\n"); projection.print();

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

        Vertex makeSphereVertex( const Vector4 & center, float radius,
                                 float lat, float lon )
        {
            Vertex v;

            float xunit = cosf(lat) * cosf(lon);
            float yunit = cosf(lat) * sinf(lon);
            float zunit = sinf(lat);

            v.position = { .x = center.x + radius * xunit,
                           .y = center.y + radius * yunit,
                           .z = center.z + radius * zunit };
            v.normal = { .x = xunit, .y = yunit, .z = zunit };

            return v;
        }

        virtual void buildGpuBuffers(ShaderProgram & shaderProgram)
        {
            glGenVertexArrays(1, &vertexArray);
            glGenBuffers(1, &vertexBuffer);
            glBindVertexArray(vertexArray);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

            unsigned int numSlices = 10;
            unsigned int numSteps = 20;
            std::vector<Vertex> vertices;
            vertices.reserve(numSlices * numSteps);

            auto const & center = obj.center;
            float radius = obj.radius;
            float dlat = (float) 1.0 / numSlices * M_PI;
            float dlon = (float) 1.0 / numSteps * 2.0 * M_PI;
            float lat = -M_PI * 0.5;
            for(unsigned int slice = 0; slice < numSlices; slice++, lat += dlat) {
                float lon = 0.0;
                for(unsigned int step = 0; step < numSteps; step++, lon += dlon) {
                    vertices.push_back( makeSphereVertex(center, radius, lat,        lon) );
                    vertices.push_back( makeSphereVertex(center, radius, lat,        lon + dlon) );
                    vertices.push_back( makeSphereVertex(center, radius, lat + dlat, lon) );
                    vertices.push_back( makeSphereVertex(center, radius, lat + dlat, lon) );
                    vertices.push_back( makeSphereVertex(center, radius, lat + dlat, lon + dlon) );
                    vertices.push_back( makeSphereVertex(center, radius, lat,        lon + dlon) );
                }
            }
            numVertices = vertices.size();

            glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

            auto positionLoc = shaderProgram.attribLocation("position");
            glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position));
            glEnableVertexAttribArray(positionLoc);
            GL_WARN_IF_ERROR();

            auto normalLoc = shaderProgram.attribLocation("normal");
            glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
            glEnableVertexAttribArray(normalLoc);
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

        virtual void buildGpuBuffers(ShaderProgram & shaderProgram)
        {
            glGenVertexArrays(1, &vertexArray);
            glGenBuffers(1, &vertexBuffer);
            glGenBuffers(1, &indexBuffer);
            glBindVertexArray(vertexArray);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            vertices.reserve(obj.vertices.size());
            indices.reserve(obj.triangles.size() * 3);

            for( unsigned int index = 0; index < obj.vertices.size(); index++ ) {
                const auto & vertex = obj.vertices[index];
                const auto & normal = obj.normals[index];
                vertices.push_back( {
                    .position = { .x = vertex.x,
                                  .y = vertex.y,
                                  .z = vertex.z },
                    .normal = { .x = normal.x,
                                .y = normal.y,
                                .z = normal.z } } );
            }
            numVertices = vertices.size();

            for( const auto & tri : obj.triangles ) {
                indices.push_back(tri.vi[0]);
                indices.push_back(tri.vi[1]);
                indices.push_back(tri.vi[2]);
            }
            numIndices = indices.size();

            glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

            auto positionLoc = shaderProgram.attribLocation("position");
            glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position));
            glEnableVertexAttribArray(positionLoc);
            GL_WARN_IF_ERROR();

            auto normalLoc = shaderProgram.attribLocation("normal");
            glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
            glEnableVertexAttribArray(normalLoc);
            GL_WARN_IF_ERROR();

            glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);
            GL_WARN_IF_ERROR();

            glBindVertexArray(0);
        }

        TriangleMesh & obj;
};

class AxisAlignedSlabEditor : public ObjectEditor {
    public:
        AxisAlignedSlabEditor(AxisAlignedSlab & o) : obj(o) {}
        ~AxisAlignedSlabEditor() {}

        virtual std::string label() { return "Box"; }
        virtual Traceable & object() const { return obj; }

        virtual void buildGpuBuffers(ShaderProgram & shaderProgram)
        {
            glGenVertexArrays(1, &vertexArray);
            glGenBuffers(1, &vertexBuffer);
            glBindVertexArray(vertexArray);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

            std::vector<Vertex> vertices;
            vertices.reserve(6 * 6);

            std::vector<Vertex::Position> corners;
            corners.push_back( { .x = obj.xmin, .y = obj.ymin, .z = obj.zmin } );
            corners.push_back( { .x = obj.xmax, .y = obj.ymin, .z = obj.zmin } );
            corners.push_back( { .x = obj.xmin, .y = obj.ymax, .z = obj.zmin } );
            corners.push_back( { .x = obj.xmax, .y = obj.ymax, .z = obj.zmin } );
            corners.push_back( { .x = obj.xmin, .y = obj.ymin, .z = obj.zmax } );
            corners.push_back( { .x = obj.xmax, .y = obj.ymin, .z = obj.zmax } );
            corners.push_back( { .x = obj.xmin, .y = obj.ymax, .z = obj.zmax } );
            corners.push_back( { .x = obj.xmax, .y = obj.ymax, .z = obj.zmax } );

            uint32_t indices[] = { // side
                0, 1, 2, 1, 3, 2,  // zmin
                0, 1, 4, 1, 5, 4,  // ymin
                2, 3, 6, 3, 7, 6,  // ymax
                4, 5, 6, 5, 7, 6,  // zmax
                2, 0, 6, 0, 4, 6,  // xmin
                1, 3, 5, 3, 7, 5,  // xmax
            };

            std::vector<Vertex::Normal> faceNormals;
            faceNormals.push_back( { .x =  0, .y =  0, .z = -1 } );
            faceNormals.push_back( { .x =  0, .y = -1, .z =  0 } );
            faceNormals.push_back( { .x =  0, .y = +1, .z =  0 } );
            faceNormals.push_back( { .x =  0, .y =  0, .z = +1 } );
            faceNormals.push_back( { .x = -1, .y =  0, .z =  0 } );
            faceNormals.push_back( { .x = +1, .y =  0, .z =  0 } );

            unsigned int cornerIndex = 0;
            for(unsigned int face = 0; face < 6; face++) {
                for(unsigned int vertIndex = 0; vertIndex < 6; vertIndex++) {
                    vertices.push_back( { .position = corners[indices[cornerIndex]],
                                          .normal = faceNormals[face] } );
                    cornerIndex++;
                }
            }

            numVertices = vertices.size();

            glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

            auto positionLoc = shaderProgram.attribLocation("position");
            glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position));
            glEnableVertexAttribArray(positionLoc);
            GL_WARN_IF_ERROR();

            auto normalLoc = shaderProgram.attribLocation("normal");
            glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
            glEnableVertexAttribArray(normalLoc);
            GL_WARN_IF_ERROR();

            glBindVertexArray(0);
        }

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


