// This may look like C code, but it is really -*- C++ -*-

// TaperedCyl.cc

// (C)  Tom White

//#include "TaperedCyl/TaperedCyl.h"
#include <allIncludes.h>

//////////////////////
// class TaperedCyl //
//////////////////////

// definitions of all member functions

TaperedCyl::TaperedCyl()
: center(0,0,0), radius(1) 
{}

TaperedCyl::TaperedCyl(double s):
  center(0,0,0), radius(s)
{
}

// initialize with center and radius
TaperedCyl::TaperedCyl(const Point3Dd& from, double rad)
: center(from), radius(rad) 
{}

// initialize with co-ordinates of two points
TaperedCyl::TaperedCyl(double x1, double y1, double z1, double rad)
  :center(x1,y1,z1), radius(rad) {}

// copy constructor
TaperedCyl::TaperedCyl(const TaperedCyl& other)
{
  *this = other;
}

// assignment operator
TaperedCyl& TaperedCyl::operator=(const TaperedCyl& other)
{
  if (this != &other) {
    center = other.center;
    radius = other.radius;
  }
  return *this;
}

// destructor
TaperedCyl::~TaperedCyl() {}

// TaperedCyl operations

// Create or clone a sphere
TaperedCyl* TaperedCyl::create() {
  return new TaperedCyl();
}

TaperedCyl* TaperedCyl::clone() {
  return new TaperedCyl(*this);
}

// Report smallest non-negative t-value of intersection or return -1
double TaperedCyl::closestIntersect(Ray& r) const
{
  // if a ray intersects a point on the cylinder, it does it where
  // compute coefficients of quadratic
  #define Px r.src.x
  #define Dx r.dir.x
  #define Py r.src.y
  #define Dy r.dir.y
  #define Pz r.src.z
  #define Dz r.dir.z
  #define s radius

  double sm = s - 1;
  double fDir = sm * Dz;
  double fStart = sm * Dz + 1;
  
  double A = 
    Dx*Dx + Dy*Dy - sm*Dz;
  double B = 
    Px*Dx + Py*Dy - ((1+sm*Pz)*(sm*Pz));
  double C = 
    Px*Px + Py*Py - (1+(sm*Pz))*(1+(sm*Pz));
  #undef Px
  #undef Py
  #undef Pz
  #undef Dx
  #undef Dy
  #undef Dz
  #undef s

  int wall1=0; //0 for walls, 1 for bottom, 2 for top
  int wall2=0;
  int wall3=0;
  int wall4=0;
  double t = -1; // assume no intersection until proven wrong
  double t1,t2, disc_root;
  int numHits=0;

  double discrim = B*B - A*C;
  if(discrim > 0.0)
    {
      disc_root = (double)sqrt(double(discrim));
      t1 = (-B - disc_root)/A; //earlier hit
      float zHit = r.src.z + r.dir.z * t1; //z component of ray
      if(t1>0.00001 && zHit <= 1.0 && zHit >=0) //if we hit a wall
	{
	  wall1=0;
	  numHits++;
	}
      t2 = (-B + disc_root)/A; //second hit
      zHit = r.src.z + r.dir.z * t2; //z component of ray
      if(t2>0.00001 && zHit <= 1.0 && zHit >=0) //if we hit a wall
	{
	  wall2=0;
	  numHits++;
	}
    }// end if (discrim>0)

  //test base at z=0
  float tb = -r.src.z/r.dir.z; //hit time at z=0 plane
  if(tb>0.00001 && SQR(r.src.x+r.dir.x*tb) + SQR(r.src.y+r.dir.y*tb) <
  1)
    //within disc of base
    {
      wall3 = 1;
      numHits++;
    }

  //test cap at z=1
  float tc = (1 - r.src.z)/r.dir.z; //hit time at z=1
  if(tc > 0.00001 && SQR(r.src.x+r.dir.x*tc) + SQR(r.src.y+r.dir.y*tc)
     < SQR(radius))
    { //within disc
      wall4 = 2;
      numHits++;
    }

  if(!numHits) return -1;
  t=-1;
  if(tc<tb&&tc>0) t=tc;
  else t=tb;
  if(t1<t&&t1>0) t=t1;
  if(t2<t&&t2>0) t=t2;
  if((t==t2)||(t==t1))
    {
      lastNormal.x = r.GetPointAt(t2).x;
      lastNormal.y = r.GetPointAt(t2).y;
      lastNormal.z = -sm* (1 + sm * r.GetPointAt(t2).z);
    }
  else if(t==tb)
    {
      lastNormal.x=0;
      lastNormal.y=0;
      lastNormal.z=-1;
    }
  else if(t==tc)
    {
      lastNormal.x=0;
      lastNormal.y=0;
      lastNormal.z=1;
    }
  if(t < 0) {
    t = -1;
  }
  return t;
}

//get normal in local coordinate system
Ray TaperedCyl::getNormal(Ray& r) const
{
  Point4Dd p4 = r.GetPointAt(closestIntersect(r));
  return Ray(p4,Point4Dd(lastNormal,1));
}

// get the radius
double TaperedCyl::GetRadius(void) const
{
  return radius;
}

// set the radius
void TaperedCyl::SetRadius(double rad)
{
  radius = rad;
}

// output function
std::ostream& TaperedCyl::out(std::ostream& o) const
{
  o << radius;
}

// input function
// get TaperedCyl from keyboard: ((x,y,z),r)
std::istream & TaperedCyl::in(std::istream& is)
{
  is >> radius;
  return is;
}
  
// Non-member functions for the type

// binary input operator: version 2
std::istream& operator>>(std::istream & is, TaperedCyl& s)
{
  return s.in(is);
}

// binary output operator: version 2
std::ostream& operator<<(std::ostream & o, const TaperedCyl& s)
{
  return s.out(o);
}


