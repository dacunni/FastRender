/*
 *  EditorSceneGraph.h
 *  FastRender
 *
 */

#ifndef _EDITOR_SCENE_GRAPH_H_
#define _EDITOR_SCENE_GRAPH_H_

#include <vector>

class Scene;
class Traceable;

class ObjectEditor {
    public:
        ObjectEditor();
        ~ObjectEditor();

        virtual std::string label();
        virtual void draw();
};

struct EditorSceneGraphNode {
    EditorSceneGraphNode(ObjectEditor * e) : editor(e) {}
    ~EditorSceneGraphNode() {}

    virtual void print(unsigned int depth = 0);

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

    EditorSceneGraphNode * root;
};



#endif

