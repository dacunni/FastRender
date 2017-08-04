#include <iostream>
#include "EditorSceneGraph.h"
#include "Scene.h"
#include "Traceable.h"
#include "TraceableVisitor.h"
#include "FlatContainer.h"
#include "Sphere.h"
#include "AxisAlignedSlab.h"
#include "TriangleMesh.h"

ObjectEditor::ObjectEditor() {}
ObjectEditor::~ObjectEditor() {}
void ObjectEditor::draw() {}
std::string ObjectEditor::label() { return "Object"; }


class SphereEditor : public ObjectEditor {
    public:
        SphereEditor(Sphere & o) : obj(o) {}
        ~SphereEditor() {}

        virtual std::string label() { return "Sphere"; }
        virtual void draw() {}

        Sphere & obj;
};

class FlatContainerEditor : public ObjectEditor {
    public:
        FlatContainerEditor(FlatContainer & o) : obj(o) {}
        ~FlatContainerEditor() {}

        virtual std::string label() { return "Flat Container"; }
        virtual void draw() {}

        FlatContainer & obj;
};

class TriangleMeshEditor : public ObjectEditor {
    public:
        TriangleMeshEditor(TriangleMesh & o) : obj(o) {}
        ~TriangleMeshEditor() {}

        virtual std::string label() { return "Triangle Mesh"; }
        virtual void draw() {}

        TriangleMesh & obj;
};

class AxisAlignedSlabEditor : public ObjectEditor {
    public:
        AxisAlignedSlabEditor(AxisAlignedSlab & o) : obj(o) {}
        ~AxisAlignedSlabEditor() {}

        virtual std::string label() { return "Box"; }
        virtual void draw() {}

        AxisAlignedSlab & obj;
};

void EditorSceneGraphNode::print(unsigned int depth) {
    std::cout << std::string(depth, '\t') << "EditorSceneGraphNode (" << editor->label() << ")" << std::endl;
    for(auto & child : children) {
        child->print(depth + 1);
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

