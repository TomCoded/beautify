#include <allIncludes.h>

//default constructor
Hit::Hit():
  point(0,0,0), surface(0), normal(ORIGIN,ORIGIN),
  sampleRay(ORIGIN,ORIGIN), tClose(0)
{}

Hit::Hit(Point3Dd point, std::shared_ptr<Surface> surface, Ray normal, Ray sampleRay,
	 double tClose):
  tClose(tClose)
{
  this->point = point;
  this->surface = surface;
  this->normal = normal;
  this->sampleRay = sampleRay;
}

//destructor
Hit::~Hit()
{}
