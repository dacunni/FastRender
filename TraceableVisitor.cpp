#include <iostream>
#include "TraceableVisitor.h"
#include "Traceable.h"

TraceableVisitor::TraceableVisitor() {}
TraceableVisitor::~TraceableVisitor() {}

void TraceableVisitor::walk( Traceable & t ) { t.visit(*this); }

void TraceableVisitor::handle( Traceable & t )               { std::cout << "WARNING: Traceable *default* handler - " << t.className() << std::endl; }

#define TV_HANDLE_IMPL(TRACEABLE_CLASS) \
    void TraceableVisitor::handle( TRACEABLE_CLASS & t ) { std::cout << "WARNING: " << #TRACEABLE_CLASS << " default handler" << std::endl; }

TV_HANDLE_IMPL( FlatContainer )
TV_HANDLE_IMPL( Sphere )
TV_HANDLE_IMPL( AxisAlignedSlab )
TV_HANDLE_IMPL( TriangleMesh )
TV_HANDLE_IMPL( BoundingVolumeHierarchy )
TV_HANDLE_IMPL( BoundingVolume )
TV_HANDLE_IMPL( CircleAreaLight )
TV_HANDLE_IMPL( RectangleAreaLight )

