#ifndef HIT_H_
#define HIT_H_

class Surface;

#include <allIncludes.h>

class Hit
{
 public:
  //constructors
  Hit(); 
  Hit(Point3Dd point, Surface * surface, Ray normal, Ray sampleRay,
  double tClose);

  //destructors
  ~Hit();

  Point3Dd point; //the hitpoint of the intersection
  Surface * surface; //the surface that is intersected.  This includes
  //both a shape and a shader.
  Ray normal;
  Ray sampleRay;
  //the sampleRay is necessary information for some lighting models
  double tClose;
};

#endif
