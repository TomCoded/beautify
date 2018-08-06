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
  //  std::cout << "Creating Square Light " << this <<std::endl;
  specular = Point3Dd(1.0,1.0,1.0);
  diffuse = Point3Dd(1.0,1.0,1.0);
  power=1e3;
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
  power=1e3;
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
  power=1e3;
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
  power = other.power;
}

SquareLight& SquareLight::operator=(const SquareLight& other) {
  if (this != &other) {
    direction=other.direction;
    diffuse=other.diffuse;
    specular=other.specular;
    power=other.power;
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
  //  std::cout << "Processing Square Light " << this <<std::endl;
  double x, y, z;

  double denom = diffuse.x+diffuse.y+diffuse.z;
  float xPow = (power * (diffuse.x / denom)) / numPhotons;
  float yPow = (power * (diffuse.y / denom)) / numPhotons;
  float zPow = (power * (diffuse.z / denom)) / numPhotons;
  long seed=SEED;
  
  g_photonPower.x = xPow;
  g_photonPower.y = yPow;
  g_photonPower.z = zPow;

  Photon p;
  Point3Dd pDir;
  for(int nEmitted=0;
      nEmitted < numPhotons;
      nEmitted++)
    {
      srand48(seed);
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
	std::cout << p <<std::endl;

      //preserve same randomness for next frame's next photon
      seed = (long)(drand48() * (1<<30));

      //trace photon
      p = renderer->tracePhoton(p);
      if(!((p.r==p.g)&&(p.g==p.b)&&(p.g==0))) {
	map->addPhoton(p);
      }
    }
}

// append point to end of stream

std::ostream& SquareLight::out(std::ostream& os) const {
  os << '(' 
     << position
     << ',' 
     << diffuse 
     << ',' 
     << specular
     << ','
     << power
     << ','
     << normal
     << ',' 
     << dx
     << ',' 
     << dy
     << ',' 
     << dz
     << ")";
  return os;
}

// read array from stream

std::istream& SquareLight::in(std::istream& is) {
  char c;
  is >> c;
  string formatErr = "Bad format for SquareLight\n Format: (position,diffuse,specular,power,normal,dx,dy,dz)";
  FORMATTEST(c,'(',formatErr)
  is >> diffuse >> c;
  FORMATTEST(c,',',formatErr)
  is >> position >> c;
  FORMATTEST(c,',',formatErr)
  is >> specular >> c;
  FORMATTEST(c,',',formatErr)
  is >> power >> c;
  FORMATTEST(c,',',formatErr)
  is >> normal >> c;
  FORMATTEST(c,',',formatErr)
  is >> dx >> c;
  FORMATTEST(c,',',formatErr)
  is >> dy >> c;
  FORMATTEST(c,',',formatErr)
  is >> dz >> c;
  FORMATTEST(c,')',formatErr)

  return is;
}

// friends
// read a SquareLight:  a binary operator
std::istream& operator>>(std::istream& is, SquareLight& l)
{
  l.in(is);
}

// print a SquareLight: a binary operator
std::ostream& operator<<(std::ostream& o, const SquareLight& l)
{
  return l.out(o);
}

