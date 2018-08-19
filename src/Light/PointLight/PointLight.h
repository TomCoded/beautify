// Light.h

// test the graphics package

// (C)  Anonymous1
// Modified 10/8/ Tom White - change from Light to LightAtInfinity
// Modifies 10/27/ Tom White - change to PointLight

#ifndef POINTLIGHT_H_
#define POINTLIGHT_H_

#include <iostream>
#include <allIncludes.h>

// A point light source at infinity
class PointLight : public Light {
public:  
  Point3Dd location;  // location of Light Source

  // Constructors
  // Default Constructor: direction "up" (0,1,0), full intensity 
  PointLight();

  // Construct PointLight using given values
  PointLight(const Point3Dd& location, const Point3Dd& diff,
	const Point3Dd& spec);

  // Copy constructor
  PointLight(const PointLight& other);

  // Assignment
  PointLight& operator=(const PointLight& other);

  // get ray from dirlight to point
  Ray getRayTo(const Point3Dd&) const;

  // gets distance to a given point from the light.
  double getDistance(const Point3Dd& point) const;

  PointLight * transparent(double transparency);

  void addPhotonsToMap(int numPhotons,
		       PhotonMap *,
		       Renderer *
			 ) {}


  // Destructor
  ~PointLight();

  // append PointLight to end of stream
  std::ostream& out(std::ostream& os) const;
    
  // read PointLight from stream
  std::istream& in(std::istream& is);
};
// friends
// read a PointLight:  a binary operator
std::istream& operator>>(std::istream& is, PointLight& m);

// print a PointLight: a binary operator
std::ostream& operator<<(std::ostream& o, const PointLight& m);

#endif POINTLIGHT_H_
