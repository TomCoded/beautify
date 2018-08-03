// This may look like C code, but it is really -*- C++ -*-

// Sphere.h

// (C) 2002 Anonymous1


#ifndef SPHERE_H
#define SPHERE_H

#include <iostream>
#include "Shape/Shape.h"
#include "Point3Dd.h"

////////////////////
// class Sphere //
////////////////////

// A closed Sphere
//

class Sphere : public Shape {

public:
  // Note that methods are no longer virtual since I do not plan extending
  // this class

  // essentials

  // default constructor
  Sphere();

  // initialize with center and radius
  Sphere(const Point3Dd& center, const double rad);

  // initialize with co-ordinates of center and radius
  Sphere(double x1, double y1, double z1, double rad);

  // copy constructor
  Sphere(const Sphere& other);

  // assignment operator
  Sphere& operator=(const Sphere& other);

  // destructor
  ~Sphere();

  // Sphere features:  Type specific operations

  // Create or clone a sphere
  Sphere* create();
  Sphere* clone();

  //implicit function
  double implicit(Point3Dd &loc) const;

  // Report smallest non-negative t-value of intersection or return -1
  double closestIntersect(Ray& r) const;

  // returns the normal of the shape with the closest intersection of
  // r with shape.
  Ray getNormal(Ray& r) const;

  // get the radius
  double GetRadius(void) const;

  // set the radius
  void SetRadius(double);

  // usually, but not always, appropriate
  ostream & out(ostream& o) const;

  // get Sphere from keyboard: ((x,y,z),(x,y,z))
  istream & in(istream& is);

private:
  Point3Dd center;
  double radius;
};

// Non-member functions for the type

// binary input operator: version 2
istream& operator>>(istream & is, Sphere& s);

// binary output operator: version 2
ostream& operator<<(ostream & o, const Sphere& s);

#endif SPHERE_H




