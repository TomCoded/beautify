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
  cout << "Power = " 
       << power.x << ','
       << power.y << ','
       << power.z << ','
       << endl;
  float xPow = ( power.x / numPhotons );
  float yPow = ( power.y / numPhotons );
  float zPow = ( power.z / numPhotons );
  cout << "power/photon = " 
       << xPow << ','
       << yPow << ','
       << zPow << ','
       << endl;
  Point3Dd normal=Point3Dd(-direction.x,-direction.y,-direction.z);
  Point3Dd point = direction * tInfinite;
  Photon p;
  for(int nEmitted=0;
      nEmitted < numPhotons;
      nEmitted++)
    {
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
      //trace photon
      p = renderer->tracePhoton(p);
      if(!((p.r==p.g)&&(p.g==p.b)&&(p.g==0)))
	map->addPhoton(p);
    }
}

// append point to end of stream

ostream& DirLight::out(ostream& os) const {
  os << "(" << diffuse << "," << specular << ',' << tInfinite 
     << ',' << dx
     << ',' << dy
     << ',' << dz
     << ")";
  return os;
}

// read array from stream

istream& DirLight::in(istream& is) {
  char c;
  is >> c;
  if (c != '(') {
    cout << "Bad format for Point3Dd" << endl;
    exit(1);
  }
  is >> diffuse >> c;
  if (c != ',') {
    cout << "Bad format for Point3Df" << endl;
    exit(1);
  }
  is >> specular >> c;
  if (c != ',') {
    cout << "Bad format for Point3Df" << endl;
    exit(1);
  }
  is >> direction >> c;
  if (c != ',') {
    cout << "Bad format for Point3Df" << endl;
    exit(1);
  }
  is >> tInfinite >> c;
  if (c != ',') {
    cout << "Bad format for Point3Df" << endl;
    exit(1);
  }
  is >> dx >> c;
  if (c != ',') {
    cout << "Bad format for Point3Df" << endl;
    exit(1);
  }
  is >> dy >> c;
  if (c != ',') {
    cout << "Bad format for Point3Df" << endl;
    exit(1);
  }
  is >> dz >> c;
  if (c != ')') {
    cout << "Bad format for Point3Df" << endl;
    exit(1);
  }
  direction=direction.normalize();
  //diffuse is power per cubic or square unit; compute total power
  double mult=1;
  if(dx) mult=dx;
  if(dy) mult=mult*dy;
  if(dz) mult=mult*dz;
  power = diffuse*mult;
  return is;
}

// friends
// read a DirLight:  a binary operator
istream& operator>>(istream& is, DirLight& l)
{
  l.in(is);
}

// print a DirLight: a binary operator
ostream& operator<<(ostream& o, const DirLight& l)
{
  return l.out(o);
}

