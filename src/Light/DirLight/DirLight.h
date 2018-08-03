// Light.h

// test the graphics package

// (C) 2002 Anonymous1
// Modified 10/8/2002 Tom White - change from Light to LightAtInfinity

#ifndef DIRLIGHT_H_
#define DIRLIGHT_H_

#include <iostream>
#include <allIncludes.h>

// A point light source at infinity
class DirLight : public Light {
public:  
  Point3Dd direction;  // direction (towards) light source
  double tInfinite; // distance towards light source for
  //plane from which to throw photons.
  double dx; //size of plane
  double dy; 
  double dz;

  // Constructors
  // Default Constructor: direction "up" (0,1,0), full intensity 
  DirLight();

  // Construct DirLight using given values
  DirLight(const Point3Dd& dir, const Point3Dd& diff,
	const Point3Dd& spec);

  DirLight(const Point3Dd& dir, 
	   const Point3Dd& diff,
	   const Point3Dd& spec,
	   double dx,
	   double dy,
	   double tInfinite
	   );
	   

  DirLight * transparent(double transparency);

  // Copy constructor
  DirLight(const DirLight& other);

  // Assignment
  DirLight& operator=(const DirLight& other);

  void addPhotonsToMap(int numPhotons,
		       PhotonMap *,
		       Renderer *
		       );

  // get ray from dirlight to point
  Ray getRayTo(const Point3Dd&) const;

  // Destructor
  ~DirLight();

  // append DirLight to end of stream

  std::ostream& out(std::ostream& os) const;
    
  // read DirLight from stream

  std::istream& in(std::istream& is);
};
// friends
// read a DirLight:  a binary operator
istream& operator>>(std::istream& is, DirLight& m);

// print a DirLight: a binary operator
ostream& operator<<(std::ostream& o, const DirLight& m);

#endif DIRLIGHT_H_
