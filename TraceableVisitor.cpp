#include <iostream>
#include "TraceableVisitor.h"
#include "Traceable.h"

TraceableVisitor::TraceableVisitor() {}
TraceableVisitor::~TraceableVisitor() {}

void TraceableVisitor::walk( Traceable & t ) { t.visit(*this); }

void TraceableVisitor::handle( Traceable & t ) { std::cout << "WARNING: Traceable *default* handler - " << t.className() << std::endl; }
void TraceableVisitor::handle( FlatContainer & t ) { std::cout << "FlatContainer handler" << std::endl; }
void TraceableVisitor::handle( Sphere & t ) { std::cout << "Sphere handler" << std::endl; }
void TraceableVisitor::handle( AxisAlignedSlab & t ) { std::cout << "AxisAlignedSlab handler" << std::endl; }
void TraceableVisitor::handle( TriangleMesh & t ) { std::cout << "TriangleMesh handler" << std::endl; }

