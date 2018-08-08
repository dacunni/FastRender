%module FastRender

%include <std_string.i>
%include <std_shared_ptr.i>

%rename(__getitem__) *::operator[];

%ignore operator<<;
%ignore operator+;
%ignore operator+=;
%ignore operator-;
%ignore operator-=;
%ignore operator*;
%ignore operator*=;

%ignore *::print;


%feature("python:slot", "tp_str", functype="reprfunc") Vector4::toString;
%feature("python:slot", "tp_str", functype="reprfunc") Matrix4x4::toString;
%feature("python:slot", "tp_str", functype="reprfunc") Transform::toString;
%feature("python:slot", "tp_str", functype="reprfunc") Ray::toString;
%feature("python:slot", "tp_str", functype="reprfunc") Traceable::toString;
%feature("python:slot", "tp_str", functype="reprfunc") Sphere::toString;
%feature("python:slot", "tp_str", functype="reprfunc") TriangleMesh::toString;

%{
#include "Vector.h"
#include "Ray.h"
#include "Traceable.h"
#include "Scene.h"
#include "Sphere.h"
#include "AssetLoader.h"
#include "TriangleMesh.h"
#include "Transform.h"
#include "Matrix.h"
%}

%shared_ptr(TriangleMesh);

%include "Vector.h"
%include "Ray.h"
%include "Traceable.h"
%include "Scene.h"
%include "Sphere.h"
%include "AssetLoader.h"
%include "TriangleMesh.h"
%include "Transform.h"
%include "Matrix.h"


