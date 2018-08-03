// This may look like C code, but it is really -*- C++ -*-

// Plane.h

// (C) 2002 Anonymous1


#ifndef PLANE_H
#define PLANE_H

#include <iostream>
#include <allIncludes.h>

////////////////////
// class Plane //
////////////////////

// A closed Plane
//

class Plane : public Shape {

public:

  // essentials

  // default constructor
  Plane();

  // initialize with point and normal
  Plane(const Point3Dd& pt, const Point3Dd& norm);

  // initialize with co-ordinates of point and normal
  Plane(double px, double py, double pz, double nx, double ny, double nz);

  // copy constructor
  Plane(const Plane& other);

  // assignment operator
  Plane& operator=(const Plane& other);

  // destructor
  ~Plane();

  // Plane features:  Type specific operations
  // create or clone a plane
  Plane* create();
  Plane* clone();

  // Report smallest non-negative t-value of intersection or return -1
  double closestIntersect(Ray& r) const;

  double implicit(Point3Dd &loc) const;

  // Generate normal
  Ray getNormal(Ray& r) const;

  // usually, but not always, appropriate
  ostream & out(ostream& o) const;

  // get Plane from keyboard: ((x,y,z),(x,y,z))
  istream & in(istream& is);

private:
  Point3Dd point;
  Point3Dd normal;
  Point3Dd bound1;
  Point3Dd bound2;
};

// Non-member functions for the type

// binary input operator: version 2
istream& operator>>(istream & is, Plane& p);

// binary output operator: version 2
ostream& operator<<(ostream & o, const Plane& p);

#endif PLANE_H




