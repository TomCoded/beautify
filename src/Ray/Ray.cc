// This may look like C code, but it is really -*- C++ -*-

// Ray.C

// (C) 2002 Anonymous1

#include "Ray/Ray.h"
#include "Point3Dd.h"

//////////////////////
// class Ray //
//////////////////////

// definitions of all member functions

Ray::Ray()
: src(0,0,0,1), dir(1,0,0,0) {}

// initialize with two points
Ray::Ray(const Point4Dd& source, const Point4Dd& direction)
  : src(source), dir(direction) {}

// initialize with co-ordinates of two points
Ray::Ray(double srcX, double srcY, double srcZ, 
	     double dirX, double dirY, double dirZ)
  :src(srcX,srcY,srcZ, 1), dir(dirX, dirY, dirZ, 0) {}

Ray::Ray(const Point3Dd& source, const Point3Dd& direction):
  src(source,1), dir(direction,0)
{}

Ray::Ray(const Ray& other)
{
  *this = other;
}

Ray& Ray::operator=(const Ray& other)
{
  if (this != &other) {
    src = other.src;
    dir = other.dir;
  }
  return *this;
}

Ray::~Ray() {}

// Ray operations

// output function
ostream& Ray::out(std::ostream& os) const
{
  Point3Dd src3 = src.dehomogenize();
  Point3Dd dir3 = dir.dehomogenize();
  os << "(" << src3 << "," << dir3 << ")";
  return os;
}

// input function
// get Ray from keyboard: ((x,y,z),(x,y,z))
istream & Ray::in(std::istream& is)
{
  char c;
  Point3Dd inputPt;

  is >> c;
  if (c != '(') {
    cout << "Bad format for Ray" << endl;
    exit(1);
  }
  is >> inputPt >> c;
  src = Point4Dd(inputPt,1);
  if (c != ',') {
    cout << "Bad format for Ray" << endl;
    exit(1);
  }
  is >> inputPt >> c;
  dir = Point4Dd(inputPt,0);
  if (c != ')') {
    cout << "Bad format for Ray" << endl;
    exit(1);
  }
  return is;
}
  
// Ray features

// Return point of ray corresponding to t
// Pre: t is non-negative (no bounds checking)
Point4Dd Ray::GetPointAt(double t) const
{
  return src + dir*t;
}

// Return t value of point on line determined by ray closest to pt
double Ray::TofClosestPoint(const Point4Dd& pt) const
{
  return dir.dot(pt - src)/dir.dot(dir);
}

// Return point on line determined by ray closest to pt
Point4Dd Ray::ClosestPoint(const Point4Dd& pt) const
{
  return GetPointAt(TofClosestPoint(pt));
}

// apply a transform to a ray
Ray& Ray::applyToSelf(const Transform4Dd& tr) {
  src.applyToSelf(tr);
  dir.applyToSelf(tr);
  return *this;
}

// Non-member functions for the type

// binary input operator
istream& operator>>(std::istream & is, Ray& theRay)
{
  return theRay.in(is);
}

// binary output operator
ostream& operator<<(std::ostream & os, const Ray& theRay)
{
  return theRay.out(os);
}


