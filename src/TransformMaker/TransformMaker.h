// This may look like C code, but it is really -*- C++ -*-

// TransformMaker.h

// Some useful conversions between point classes

// (C) Anonymous1 2002

#ifndef TRANSFORMMAKER_H
#define TRANSFORMMAKER_H

#include "Point3Dd.h"
#include "Transform4Dd.h"

#include <cmath>

//const double PI = 3.14159;
#ifndef PI
#define PI 3.14159
#endif

// All angles are in radians:  2*PI radians = 360 degrees

// Return an array, in column major order, holding entries of transform
double * GetArray(const Transform4Dd& tr);

// A transform that rotates vec onto dir around (vec x dir)
Transform4Dd MakeRotation(const Point3Dd& vec, const Point3Dd& dir);

// Make a counter-clockwise rotation of angle theta about std::vector d
Transform4Dd MakeRotation(double theta, const Point3Dd& d);

// Make a positive rotation of angle theta about the x axis
Transform4Dd MakeXRotation(double theta);
  
// Make a positive rotation of angle theta about the y axis
Transform4Dd MakeYRotation(double theta);
  
// Make a positive rotation of angle theta about the z axis
Transform4Dd MakeZRotation(double theta);

// Make a  rotation sending b1,b2,b3 (orthonormal) to x,y,z axes
Transform4Dd MakeRotation(const Point3Dd &b1,
			  const Point3Dd& b2,
			  const Point3Dd& b3);

// Translate x,y and z by p.x,p.y,p.z respectively
Transform4Dd MakeTranslation(const Point3Dd& p);

// Translate x,y and z by dx,dy, and dz respectively
Transform4Dd MakeTranslation(double dx, double dy, double dz);

// scale x,y and z by p.x,p.y,p.z respectively
Transform4Dd MakeScale(const Point3Dd& p);

// scale x,y and z by sx,sy, and sz respectively
Transform4Dd MakeScale(double sx, double sy, double sz);
  
#endif TRANSFORMMAKER_H
