// PointLight.cc

// test the graphics package

// (C) 2002 Tom White

#include "allIncludes.h"

//////////////////////
// class PointLight   //
//////////////////////

// A point light source at infinity
//Constructors
// Default Constructor: direction "up" (0,1,0), full intensity 
PointLight::PointLight():
  location(0.0,1.0,0.0)
{
  power=Point3Dd(0,0,0);
  specular = Point3Dd(1.0,1.0,1.0);
  diffuse = Point3Dd(1.0,1.0,1.0);
  specularCo=1;
}

// Construct PointLight using given values
PointLight::PointLight(const Point3Dd& loc, const Point3Dd& diff,
	     const Point3Dd& spec):
  location(loc)
{
  power=Point3Dd(0,0,0);
  specular=spec;
  diffuse = Point3Dd(diff);
}

PointLight * PointLight::transparent(double transparency)
{
  return new PointLight(location, diffuse*transparency, specular*transparency);
}


// Copy constructor
PointLight::PointLight(const PointLight& other) {
  *this = other;
}

PointLight& PointLight::operator=(const PointLight& other) {
  if (this != &other) {
    location=other.location;
    diffuse=other.diffuse;
    specular=other.specular;
  }
  return *this;
}

PointLight::~PointLight() {}

//get Ray To light source from a given point
Ray PointLight::getRayTo(Point3Dd& dest)
{
  //  Point3Dd dir = location-dest;
  Point3Dd dir = dest-location;
  return Ray(dest, dir.normalize());
}

// append point to end of stream

ostream& PointLight::out(ostream& os) const {
  os << "(" << location << "," << diffuse << "," << specular << ")";
  return os;
}

// read array from stream

istream& PointLight::in(istream& is) {
  char c;
  is >> c;
  if (c != '(') {
    cout << "Bad format for Point3Dd" << endl;
    exit(1);
  }
  is >> location >> c;
  if (c != ',') {
    cout << "Bad format for Point3Df" << endl;
    exit(1);
  }
  is >> diffuse >> c;
  if (c != ',') {
    cout << "Bad format for Point3Df" << endl;
    exit(1);
  }
  is >> specular >> c;
  if (c != ')') {
    cout << "Bad format for Point3Df" << endl;
    exit(1);
  }
  return is;
}

// friends
// read a PointLight:  a binary operator
istream& operator>>(istream& is, PointLight& l)
{
  l.in(is);
}

// print a PointLight: a binary operator
ostream& operator<<(ostream& o, const PointLight& l)
{
  return l.out(o);
}

