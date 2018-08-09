// LightAtInfinity.cc

// test the graphics package

// (C) 2002 Anonymous1
// Modified 2002 Tom White

//#include "DirLight/DirLight.h"
#include "allIncludes.h"

//////////////////////
// class DirLight   //
//////////////////////

// A point light source at infinity
//Constructors
// Default Constructor: direction "up" (0,1,0), full intensity 
DirLight::DirLight() :
  direction(0.0,1.0,0.0)
{
  specular = Point3Dd(1.0,1.0,1.0);
  diffuse = Point3Dd(1.0,1.0,1.0);
  specularCo=1;
}

// Construct DirLight using given values
DirLight::DirLight(const Point3Dd& dir, const Point3Dd& diff,
	     const Point3Dd& spec):
  direction(dir)
{
  specular=spec;
  diffuse = Point3Dd(diff),
  direction=direction.normalize();
}

DirLight::DirLight(const Point3Dd& dir,
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

DirLight * DirLight::transparent(double transparency)
{
  return new DirLight(direction, diffuse*transparency, specular*transparency);
}

// Copy constructor
DirLight::DirLight(const DirLight& other) {
  specular = other.specular;
  diffuse = other.diffuse;
  specularCo = other.specularCo;
  direction = other.direction;
}

DirLight& DirLight::operator=(const DirLight& other) {
  if (this != &other) {
    direction=other.direction;
    diffuse=other.diffuse;
    specular=other.specular;
  }
  return *this;
}

DirLight::~DirLight() {}

//get Ray To light source from a given point
Ray DirLight::getRayTo(const Point3Dd& dest) const
{
  return Ray(dest, direction);
}

void DirLight::addPhotonsToMap(int numPhotons,
		     PhotonMap * map,
		     Renderer * renderer
		     )
{
  double x, y, z;

  double denom = diffuse.x+diffuse.y+diffuse.z;
  float xPow = (power * (diffuse.x / denom)) / numPhotons;
  float yPow = (power * (diffuse.y / denom)) / numPhotons;
  float zPow = (power * (diffuse.z / denom)) / numPhotons;
  std::cout << "power/photon = " 
       << xPow << ','
       << yPow << ','
       << zPow << ','
       <<std::endl;
  Point3Dd normal=Point3Dd(-direction.x,-direction.y,-direction.z);
  Point3Dd point = direction * tInfinite;
  Photon p; long seed=SEED;
  for(int nEmitted=0;
      nEmitted < numPhotons;
      nEmitted++)
    {
      //seed random generator
      srand48(seed);
      //pick random part of plane
      x = (drand48() - 0.5)*dx;
      y = (drand48() - 0.5)*dy;
      z = (drand48() - 0.5)*dz;
      //set photon power
      p.r = xPow;
      p.g = yPow;
      p.b = zPow;
      //set photon direction
      p.dx = -direction.x;
      p.dy = -direction.y;
      p.dz = -direction.z;
      //set photon origin
      p.x = point.x+x;
      p.y = point.y+y;
      p.z = point.z+z;
      //generate random seed for next loop.
      //we do this here because drand48() will
      //be used an inconsistent number of times
      //in future frames depending on the scene
      //at any given moment in time, during the
      //photon tracing.
      seed = (long)(drand48() * (1<<30));
      //trace photon
      p = renderer->tracePhoton(p);
      if(!((p.r==p.g)&&(p.g==p.b)&&(p.g==0)))
	map->addPhoton(p);
    }
}

// append point to end of stream

std::ostream& DirLight::out(std::ostream& os) const {
  os << "(" << diffuse << "," << specular << ',' << tInfinite 
     << ',' << dx
     << ',' << dy
     << ',' << dz
     << ")";
  return os;
}

// read array from stream

std::istream& DirLight::in(std::istream& is) {
  char c;
  is >> c;
  std::string formatErr="Bad format for DirLight";
  FORMATTEST(c,'(',formatErr)
  is >> diffuse >> c;
  FORMATTEST(c,',',formatErr)
  is >> specular >> c;
  FORMATTEST(c,',',formatErr)
  is >> direction >> c;
  FORMATTEST(c,',',formatErr)
  is >> tInfinite >> c;
  FORMATTEST(c,',',formatErr)
  is >> dx >> c;
  FORMATTEST(c,',',formatErr)
  is >> dy >> c;
  FORMATTEST(c,',',formatErr)
  is >> dz >> c;
  FORMATTEST(c,')',formatErr)
  direction=direction.normalize();
  //diffuse is power per cubic or square unit; compute total power
  double mult=1;
  if(dx) mult=dx;
  if(dy) mult=mult*dy;
  if(dz) mult=mult*dz;
  power = diffuse.norm()*mult;
  return is;
}

// friends
// read a DirLight:  a binary operator
std::istream& operator>>(std::istream& is, DirLight& l)
{
  l.in(is);
}

// print a DirLight: a binary operator
std::ostream& operator<<(std::ostream& o, const DirLight& l)
{
  return l.out(o);
}

