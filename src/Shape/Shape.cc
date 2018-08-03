// This may look like C code, but it is really -*- C++ -*-

// Shape.cc

// (C) 2002 Anonymous1

#include "Shape/Shape.h"

////////////////////
// class Shape    //
////////////////////

// essentials
// There's no such thing as a virtual constructor...
// However, we might want to create or clone a derived object
Shape* Shape::create() {}
Shape* Shape::clone() {};

// destructor
Shape::~Shape() {}

// Shape features:  Type specific operations

// Report smallest non-negative t-value of intersection or return -1
double Shape::closestIntersect(Ray& r) const {};

double Shape::implicit(Point3Dd &loc) const {};

istream& Shape::in(istream&) {}

ostream& Shape::out(ostream&) const {}




