// SqaureLight.h

// A square light

#ifndef SQUARELIGHT_H_
#define SQUARELIGHT_H_

#include <iostream>
#include <allIncludes.h>

// A point light source at infinity
class SquareLight : public Light {
public:  
  Point3Dd direction;  // direction (towards) light source
  Point3Dd normal;
  Point3Dd position;
  //  double tInfinite; // distance towards light source for
  //plane from which to throw photons.
  double dx; //size of plane
  double dy; 
  double dz;

  // Constructors
  // Default Constructor: direction "up" (0,1,0), full intensity 
  SquareLight();

  // Construct SquareLight using given values
  SquareLight(const Point3Dd& dir, const Point3Dd& diff,
	const Point3Dd& spec);

  SquareLight(const Point3Dd& dir, 
	   const Point3Dd& diff,
	   const Point3Dd& spec,
	   double dx,
	   double dy,
	   double tInfinite
	   );
	   

  SquareLight * transparent(double transparency);

  // Copy constructor
  SquareLight(const SquareLight& other);

  // Assignment
  SquareLight& operator=(const SquareLight& other);

  void addPhotonsToMap(int numPhotons,
		       PhotonMap *,
		       Renderer *
		       );

  // get ray from dirlight to point
  Ray getRayTo(const Point3Dd&) const;

  // Destructor
  ~SquareLight();

  // append SquareLight to end of stream

  std::ostream& out(std::ostream& os) const;
    
  // read SquareLight from stream

  std::istream& in(std::istream& is);
};
// friends
// read a SquareLight:  a binary operator
std::istream& operator>>(std::istream& is, SquareLight& m);

// print a SquareLight: a binary operator
std::ostream& operator<<(std::ostream& o, const SquareLight& m);

#endif
