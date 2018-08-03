// SquareLight.cc
// (C) 2002 Tom White

#include <Light/SquareLight/SquareLight.h>
#include "allIncludes.h"

//////////////////////
// class SquareLight   //
//////////////////////

// A point light source at infinity
//Constructors
// Default Constructor: direction "up" (0,1,0), full intensity 
SquareLight::SquareLight() :
  direction(0.0,1.0,0.0)
{
  //  cout << "Creating Square Light " << this << endl;
  specular = Point3Dd(1.0,1.0,1.0);
  diffuse = Point3Dd(1.0,1.0,1.0);
  specularCo=1;
}

// Construct SquareLight using given values
SquareLight::SquareLight(const Point3Dd& dir, const Point3Dd& diff,
	     const Point3Dd& spec):
  direction(dir)
{
  specular=spec;
  diffuse = Point3Dd(diff),
  direction=direction.normalize();
}

SquareLight::SquareLight(const Point3Dd& dir,
		   const Point3Dd& diff,
		   const Point3Dd& spec,
		   double dx,
		   double dy,
		   double dz
		   ):
  direction(dir),
  dx(dx), dy(dy), dz(dz)
{
  specular=spec;
  diffuse=diff;
  direction=direction.normalize();
}

SquareLight * SquareLight::transparent(double transparency)
{
  return new SquareLight(direction, diffuse*transparency, specular*transparency);
}

// Copy constructor
SquareLight::SquareLight(const SquareLight& other) {
  specular = other.specular;
  diffuse = other.diffuse;
  specularCo = other.specularCo;
  direction = other.direction;
}

SquareLight& SquareLight::operator=(const SquareLight& other) {
  if (this != &other) {
    direction=other.direction;
    diffuse=other.diffuse;
    specular=other.specular;
  }
  return *this;
}

SquareLight::~SquareLight() {}

//get Ray To light source from a given point
Ray SquareLight::getRayTo(const Point3Dd& dest) const
{
  return Ray(dest, direction);
}

void SquareLight::addPhotonsToMap(int numPhotons,
		     PhotonMap * map,
		     Renderer * renderer
		     )
{
  //  cout << "Processing Square Light " << this << endl;
  double x, y, z;

  float xPow = ( power.x / numPhotons );
  float yPow = ( power.y / numPhotons );
  float zPow = ( power.z / numPhotons );

  cout << "Total Power: " << power << endl;
  cout << "Power: " << xPow << ',' << yPow << ',' << zPow << endl;

  Photon p;
  Point3Dd pDir;
  for(int nEmitted=0;
      nEmitted < numPhotons;
      nEmitted++)
    {
      //pick random part of plane
      x = (drand48() - 0.5)*dx;
      y = (drand48() - 0.5)*dy;
      z = (drand48() - 0.5)*dz;
      
      //pick direction
      do
	{
	  pDir.x = (drand48() - 0.5)*2;
	  pDir.y = (drand48() - 0.5)*2;
	  pDir.z = (drand48() - 0.5)*2;
	}
      while( ( pDir.dot(normal) < 0) ||
	    (drand48() - (pDir.dot(normal) / pDir.norm() * pDir.norm()))
	     < 0
	     );

      //set photon power
      p.r = xPow;
      p.g = yPow;
      p.b = zPow;
      //set photon direction
      p.dx = pDir.x;
      p.dy = pDir.y;
      p.dz = pDir.z;
      //set photon origin
      p.x = position.x+x;
      p.y = position.y+y;
      p.z = position.z+z;

      static int counter=0;
      if(!counter++%50)
	cout << p << endl;

      //trace photon
      p = renderer->tracePhoton(p);
      if(!((p.r==p.g)&&(p.g==p.b)&&(p.g==0))) {
	map->addPhoton(p);
      }
    }
}

// append point to end of stream

ostream& SquareLight::out(ostream& os) const {
  os << "(" << diffuse 
     << "," << specular
     << ',' << position
     << ',' << normal
     << ',' << dx
     << ',' << dy
     << ',' << dz
     << ")";
  return os;
}

// read array from stream

istream& SquareLight::in(istream& is) {
  char c;
  is >> c;
  if (c != '(') {
    cout << "Bad format for SquareLight" << endl;
    exit(1);
  }
  is >> diffuse >> c;
  if (c != ',') {
    cout << "Bad format for SquareLight" << endl;
    exit(1);
  }
  is >> specular >> c;
  if (c != ',') {
    cout << "Bad format for SquareLight" << endl;
    exit(1);
  }
  is >> position >> c;
  if (c != ',') {
    cout << "Bad format for SquareLight" << endl;
    exit(1);
  }
  is >> normal >> c;
  if (c!=',') {
    cout << "Bad format for normal; should be Point3Df\n";
    exit(1);
  }
  is >> dx >> c;
  if (c != ',') {
    cout << "Bad format for Squarelight" << endl;
    exit(1);
  }
  is >> dy >> c;
  if (c != ',') {
    cout << "Bad format for Squarelight" << endl;
    exit(1);
  }
  is >> dz >> c;
  if (c != ')') {
    cout << "Bad format for Squarelight" << endl;
    exit(1);
  }

  //diffuse is power in watts of light; compute total power
  power = diffuse;

  return is;
}

// friends
// read a SquareLight:  a binary operator
istream& operator>>(istream& is, SquareLight& l)
{
  l.in(is);
}

// print a SquareLight: a binary operator
ostream& operator<<(ostream& o, const SquareLight& l)
{
  return l.out(o);
}

