#include <iostream>
#include "TraceableVisitor.h"
#include "Traceable.h"

TraceableVisitor::TraceableVisitor() {}
TraceableVisitor::~TraceableVisitor() {}

void TraceableVisitor::walk( Traceable & t ) { t.visit(*this); }

void TraceableVisitor::handle( Traceable & t )               { std::cout << "WARNING: Traceable *default* handler - " << t.className() << std::endl; }
void TraceableVisitor::handle( FlatContainer & t )           { std::cout << "WARNING: FlatContainer *default* handler" << std::endl; }
void TraceableVisitor::handle( Sphere & t )                  { std::cout << "WARNING: Sphere *default* handler" << std::endl; }
void TraceableVisitor::handle( AxisAlignedSlab & t )         { std::cout << "WARNING: AxisAlignedSlab *default* handler" << std::endl; }
void TraceableVisitor::handle( TriangleMesh & t )            { std::cout << "WARNING: TriangleMesh *default* handler" << std::endl; }
void TraceableVisitor::handle( BoundingVolumeHierarchy & t ) { std::cout << "WARNING: BoundingVolumeHierarchy *default* handler" << std::endl; }
void TraceableVisitor::handle( BoundingVolume & t )          { std::cout << "WARNING: BoundingVolume *default* handler" << std::endl; }

