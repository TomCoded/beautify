// This may look like C code, but it is really -*- C++ -*-

// Sphere.cc

// (C) 2002 Anonymous1

//#include "Sphere/Sphere.h"
#include <allIncludes.h>

//////////////////////
// class Sphere //
//////////////////////

// definitions of all member functions

Sphere::Sphere()
: center(0,0,0), radius(1) 
{}

// initialize with center and radius
Sphere::Sphere(const Point3Dd& from, double rad)
: center(from), radius(rad) 
{}

// initialize with co-ordinates of two points
Sphere::Sphere(double x1, double y1, double z1, double rad)
  :center(x1,y1,z1), radius(rad) {}

// copy constructor
Sphere::Sphere(const Sphere& other)
{
  *this = other;
}

//implicit function of a sphere
double Sphere::implicit(Point3Dd &loc) const
{
  return SQR(loc.x)+SQR(loc.y)+SQR(loc.z) - SQR(radius);
}

// assignment operator
Sphere& Sphere::operator=(const Sphere& other)
{
  if (this != &other) {
    center = other.center;
    radius = other.radius;
  }
  return *this;
}

// destructor
Sphere::~Sphere() {}

// Sphere operations

// Create or clone a sphere
Sphere* Sphere::create() {
  return new Sphere();
}

Sphere* Sphere::clone() {
  return new Sphere(*this);
}

// Report smallest non-negative t-value of intersection or return -1
double Sphere::closestIntersect(Ray& r) const
{
  // compute coefficients of quadratic
  Point3Dd src(r.src.x,r.src.y,r.src.z);
  Point3Dd dir(r.dir.x,r.dir.y,r.dir.z);
  Point3Dd diff = src - center;
  double A = dir.dot(dir);
  double B = diff.dot(dir);
  double C = diff.dot(diff) - radius*radius;

  double t = -1; // assume no intersection until proven wrong

  double discrim = B*B - A*C;
  if(discrim >= 0) { // they intersect
    // start checking roots for sign
    t = (-B - std::sqrt(discrim))/A;
    // if smaller root is negative, check larger one
    if(t < 0) {
      t = (-B + std::sqrt(discrim))/A;
      if(t < 0) t = -1;
    }
  }
  //do not return intersections behind the ray
  return t;
}

//get normal in local coordinate system
Ray Sphere::getNormal(Ray& r) const
{
  double tVal = closestIntersect(r);
  Point4Dd pt4 = r.GetPointAt(tVal);
  return Ray(center, (Point3Dd(pt4.x,pt4.y,pt4.z) - center).normalize());
}

// get the radius
double Sphere::GetRadius(void) const
{
  return radius;
}

// set the radius
void Sphere::SetRadius(double rad)
{
  radius = rad;
}

// output function
std::ostream& Sphere::out(std::ostream& o) const
{
  o << "(" << center << "," << radius << ")";
}

// input function
// get Sphere from keyboard: ((x,y,z),r)
std::istream & Sphere::in(std::istream& is)
{
  char c;

  is >> c;
  std::string formatErr = "Bad format for Sphere";
  FORMATTEST(c,'(',formatErr)
  is >> center >> c;
  FORMATTEST(c,',',formatErr)
  is >> radius >> c;
  FORMATTEST(c,')',formatErr)
  
  return is;
}
  
// Non-member functions for the type

// binary input operator: version 2
std::istream& operator>>(std::istream & is, Sphere& s)
{
  return s.in(is);
}

// binary output operator: version 2
std::ostream& operator<<(std::ostream & o, const Sphere& s)
{
  return s.out(o);
}


