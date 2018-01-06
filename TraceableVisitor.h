#ifndef _TRACEABLE_VISITOR_H_
#define _TRACEABLE_VISITOR_H_

#include <string>

class Traceable;
class FlatContainer;
class Sphere;
class AxisAlignedSlab;
class TriangleMesh;

// Visitor object for a scene graph made of Traceable objects.
//
// Traceables should implement visit() like the following:
// 
//   virtual void visit( TraceableVisitor & visitor );                            // prototype
//   void Thingy::visit( TraceableVisitor & visitor ) { visitor.handle(*this); }  // implementation
//
// Traceables should have a correpsonding handle() method here and in subclasses
// of the visitor to ensure they are handled when walking the scene graph.
//
class TraceableVisitor {
    public:
        TraceableVisitor();
        ~TraceableVisitor();

        // Call to walk a Traceable hierarchy, invoking visit() on each
        // object, which should in turn call handle() with itself as
        // an argument and walk() any child objects.
        virtual void walk( Traceable & t );

        // Subclasses should override the handle() methods to implement
        // custom behavior for each type of object.
        virtual void handle( Traceable & t );
        virtual void handle( FlatContainer & t );
        virtual void handle( Sphere & t );
        virtual void handle( AxisAlignedSlab & t );
        virtual void handle( TriangleMesh & t );
};



#endif

