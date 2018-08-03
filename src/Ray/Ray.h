// This may look like C code, but it is really -*- C++ -*-

// Ray.h

// (C) 2002 Anonymous1


#ifndef RAY_H
#define RAY_H

#include <iostream>
#include "Point4Dd.h"
#include "Transform4Dd.h"

////////////////////
// class Ray //
////////////////////

// A ray 
//

class Ray {

public:

  // essentials

  // default constructor
  Ray();

  // initialize with two points
  Ray(const Point4Dd& source, const Point4Dd& direction);

  //initialize with two points
  Ray(const Point3Dd& source, const Point3Dd& direction);

  // initialize with co-ordinates of two points
  Ray(double srcX, double srcY, double srcZ,
	double dirX, double dirY, double dirZ);

  // copy constructor
  Ray(const Ray& other);

  // assignment operator
  Ray& operator=(const Ray& other);

  // destructor
  ~Ray();

  // usually, but not always, appropriate
  std::ostream & out(std::ostream& os) const;

  // get Ray from keyboard: ((x,y,z),(x,y,z))
  std::istream & in(std::istream& is);

  // Ray features

  // the data
  Point4Dd src;
  Point4Dd dir;

  // Return point of ray corresponding to t
  // Pre: t is non-negative (no bounds checking)
  Point4Dd GetPointAt(double t) const;

  // Return t value of point on line determined by ray closest to pt
  double TofClosestPoint(const Point4Dd& pt) const;

  // Return point on line determined by ray closest to pt
  Point4Dd ClosestPoint(const Point4Dd& pt) const;

  // apply a transform to a ray
  Ray& applyToSelf(const Transform4Dd& tr);
};

// Non-member functions for the type

// binary input operator
std::istream& operator>>(std::istream& is, Ray& theRay);

// binary output operator
std::ostream& operator<<(std::ostream& os, const Ray& theRay);

#endif RAY_H




