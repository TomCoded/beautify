// This may look like C code, but it is really -*- C++ -*-

// Shape.h

// (C)  Anonymous1
// Modified  Tom White


#ifndef SHAPE_H
#define SHAPE_H

class Shape;
//class Transform4Dd;

#define COMPILINGSHAPE

// Point*D* clases now in liblinAlg
#include <allIncludes.h>

#undef COMPILINGSHAPE

////////////////////
// class Shape    //
////////////////////

// The interface for a shape object
//
class Shape {

public:

  // essentials
  // There's no such thing as a virtual constructor...
  // However, we might want to create or clone a derived object
  virtual Shape* create() = 0;
  virtual Shape* clone() = 0;

  // destructor
  virtual ~Shape() = 0;

  // Shape features:  Type specific operations

  // Report smallest non-negative t-value of intersection or return -1
  virtual double closestIntersect(Ray& r) const = 0;

  //returns the normal to the shape at the
  //closest intersection of r with shape.
  virtual Ray getNormal(Ray& r) const = 0;

  virtual std::istream& in(std::istream&) = 0;

  virtual std::ostream& out(std::ostream&) const = 0;

  //implicit function of the surface
  //at a given point
  virtual double implicit(Point3Dd &loc) const = 0;

  // No peeking below here!!!
protected:  // features which might be inherited
  
  Point3Dd shapeCenter;

};

#endif SHAPE_H




