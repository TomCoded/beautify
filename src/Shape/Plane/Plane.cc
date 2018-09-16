// This may look like C code, but it is really -*- C++ -*-

// Plane.cc

// (C)  Anonymous1

#include <Shape/Plane/Plane.h>

////////////////////
// class Plane    //
////////////////////

// A Plane

// essentials

// default constructor
Plane::Plane() : point(0,0,0), normal(0,0,1),
		 bound1(-0.5,-0.5,-0.001), 
		 bound2(0.5,0.5,0.001)
{
}

// initialize with point and normal
Plane::Plane(const Point3Dd& pt, const Point3Dd& norm) :
  point(pt), normal(norm) 
{}

// initialize with co-ordinates of point and normal
Plane::Plane(double px, double py, double pz, double nx, double ny, double nz) :
  point(px,py,pz), normal(nx,ny,nz) 
{}

// copy constructor
Plane::Plane(const Plane& other)
{
  (*this)=other;
}

// assignment operator
Plane& Plane::operator=(const Plane& other)
{
  if(this != &other) {
    point = other.point;
    normal = other.normal;
    bound1=other.bound1;
    bound2=other.bound2;
  }
  return *this;
}

// destructor
Plane::~Plane() {}

// Plane features:  Type specific operations
// Create or clone a plane
Plane* Plane::create() {
  return new Plane();
}

Plane* Plane::clone() {
  return new Plane(*this);
}

// Report smallest non-negative t-value of intersection or return negative
// number if none
double Plane::closestIntersect(Ray& r) const
{
  double t = -1;
  double denom = normal.dot(Point3Dd(r.dir.x,r.dir.y,r.dir.z));
  if (denom != 0)
    t = normal.dot(point - Point3Dd(r.src.x,r.src.y,r.src.z))/denom;
  if(t!=-1)
    { //compare to bounding box around plane
      Point4Dd hitPoint=r.GetPointAt(t);
      if(
	 (hitPoint.x < bound1.data[0])||
	 (hitPoint.x > bound2.data[0])||
	 (hitPoint.y < bound1.data[1])||
	 (hitPoint.y > bound2.data[1])||
	 (hitPoint.z < bound1.data[2])||
	 (hitPoint.z > bound2.data[2])
	 )
	t=-1;
    }
  if ( t < 0 )
    t = -1;
  return t;
}

//determine if point is within the bounding box of the plane
double Plane::implicit(Point3Dd &loc) const {
  double t;
      if(
	 (loc.x < bound1.data[0])||
	 (loc.x > bound2.data[0])||
	 (loc.y < bound1.data[1])||
	 (loc.y > bound2.data[1])||
	 (loc.z < bound1.data[2])||
	 (loc.z > bound2.data[2])
	 )
	//outside
	t=1;
      //inside
      else {
	t=-1;
      }
      return t;
}

//get normal in local coordinate system
Ray Plane::getNormal(Ray& r) const
{
  //The normal is negative if we hit the plane from the back.
  double tVal = closestIntersect(r);
  if(Point4Dd(normal,1.0).dot(r.dir) <= 0)
    return Ray(r.GetPointAt(tVal), Point4Dd(normal,0));
  else
    return Ray(r.GetPointAt(tVal), Point4Dd(normal*-1,0));
}

// usually, but not always, appropriate
std::ostream & Plane::out(std::ostream& o) const
{
  o << "(" << point << "," << normal << ")";
}

// get Plane from keyboard: ((x,y,z),(x,y,z))
std::istream & Plane::in(std::istream& is)
{
  char c;
  
  is >> c;
  std::string formatErr="Bad format for Plane";
  FORMATTEST(c,'(',formatErr)
  is >> point >> c;
  FORMATTEST(c,',',formatErr)
  is >> normal >> c;
  FORMATTEST(c,')',formatErr)
  return is;
}

// Non-member functions for the type

// binary input operator: version 2
std::istream& operator>>(std::istream & is, Plane& p);

// binary output operator: version 2
std::ostream& operator<<(std::ostream & o, const Plane& p);




