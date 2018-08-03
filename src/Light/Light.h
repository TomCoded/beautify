// Light.h

// (C) 2002 Tom White

#ifndef LIGHT_H_
#define LIGHT_H_
#define COMPILINGLIGHT

#define LIGHT_INF_DISTANCE 10000000

#include <allIncludes.h>
#include <Photon/Photon.h>
#include <PhotonMap/PhotonMap.h>
#include <Renderer/Renderer.h>

class Light {

 public:  
  //destructor:
  virtual ~Light() = 0;

  //Light features: all lights will have these

  //Adds photons to map, if the light supports it.
  virtual void addPhotonsToMap(int numPhotons,
			       PhotonMap *,
			       Renderer *
			       )=0;

  //Return power of the light in each color channel
  Point3Dd getPower() const;

  //Return distance to the light from another point
  virtual double getDistance(const Point3Dd& point) const;
  
  //Sets power of the light in each color channel
  virtual Point3Dd setPower(Point3Dd&);

  //returns the ray from the light to the point
  virtual Ray getRayTo(const Point3Dd&) const = 0;

  //returns a light that has been reduced by transparency
  virtual Light * transparent(double transparency) = 0;

  virtual istream& in(istream&) = 0;
  virtual ostream& out(ostream&) const = 0;
  
  Point3Dd ambient;
  Point3Dd diffuse;
  Point3Dd specular;
  double specularCo;

 protected:
  //features we want visible to inherited classes, but not public
  Point3Dd power; //power of the light, in pseudo-wattage
};

#undef COMPILINGLIGHT
#endif
