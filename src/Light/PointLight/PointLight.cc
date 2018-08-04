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
  //1 Kilowatt default
  power=1000;
  specular = Point3Dd(1.0,1.0,1.0);
  diffuse = Point3Dd(1.0,1.0,1.0);
  specularCo=1;
}

// Construct PointLight using given values
PointLight::PointLight(const Point3Dd& loc, const Point3Dd& diff,
	     const Point3Dd& spec):
  location(loc)
{
  power=1000;
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
Ray PointLight::getRayTo(const Point3Dd& dest) const
{
  //  Point3Dd dir = location-dest;
  Point3Dd dir = dest-location;
  return Ray(dest, dir.normalize());
}

double PointLight::getDistance(const Point3Dd& p) const {
  return (p-location).norm();
}

// append point to end of stream

std::ostream& PointLight::out(std::ostream& os) const {
  os << "(" << location << "," << diffuse << "," << specular << ")";
  return os;
}

// read array from stream

std::istream& PointLight::in(std::istream& is) {
  char c;
  is >> c;
  string formatErr = "Bad format for PointLight";
  FORMATTEST(c,'(',formatErr)
  is >> location >> c;
  FORMATTEST(c,',',formatErr)
  is >> diffuse >> c;
  FORMATTEST(c,',',formatErr)
  is >> specular >> c;
  FORMATTEST(c,')',formatErr)
  return is;
}

// friends
// read a PointLight:  a binary operator
std::istream& operator>>(std::istream& is, PointLight& l)
{
  l.in(is);
}

// print a PointLight: a binary operator
std::ostream& operator<<(std::ostream& o, const PointLight& l)
{
  return l.out(o);
}

