// This may look like C code, but it is really -*- C++ -*-

// TaperedCyl.h

// (C) 2002 Tom White


#ifndef TAPEREDCYL_H
#define TAPEREDCYL_H

#include <iostream>
#include "Shape/Shape.h"
#include "Point3Dd.h"

////////////////////
// class TaperedCyl //
////////////////////

// A closed TaperedCyl
//

class TaperedCyl : public Shape {

public:
  // essentials

  // default constructor
  TaperedCyl();

  //initialize with small radius
  TaperedCyl(double s);

  // initialize with center and radius
  TaperedCyl(const Point3Dd& center, const double rad);

  // initialize with co-ordinates of center and radius
  TaperedCyl(double x1, double y1, double z1, double rad);

  // copy constructor
  TaperedCyl(const TaperedCyl& other);

  // assignment operator
  TaperedCyl& operator=(const TaperedCyl& other);

  // destructor
  ~TaperedCyl();

  double implicit(Point3Dd &loc) const
    //FIXME
  { return -1; }

  // TaperedCyl features:  Type specific operations

  // Create or clone a sphere
  TaperedCyl* create();
  TaperedCyl* clone();

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

  // get TaperedCyl from keyboard: ((x,y,z),(x,y,z))
  istream & in(istream& is);

private:
  Point3Dd center;
  Point3Dd lastNormal;
  double radius; //small radius
};

// Non-member functions for the type

// binary input operator: version 2
istream& operator>>(istream & is, TaperedCyl& s);

// binary output operator: version 2
ostream& operator<<(ostream & o, const TaperedCyl& s);

#endif TAPEREDCYL_H




