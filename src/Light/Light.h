// Light.h

// (C)  Tom White

#ifndef LIGHT_H_
#define LIGHT_H_
#define COMPILINGLIGHT

#define LIGHT_INF_DISTANCE 10000000

#include <allIncludes.h>
#include <Photon/Photon.h>
#include <PhotonMap/PhotonMap.h>
#include <Renderer/Renderer.h>

#define SEED 22

extern Point3Dd g_photonPower;

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
  double getPower() const;

  //Return distance to the light from another point
  virtual double getDistance(const Point3Dd& point) const;
  
  //Sets power of the light in each color channel
  virtual void setPower(const double);

  //returns the ray from the light to the point
  virtual Ray getRayTo(const Point3Dd&) const = 0;

  //returns a light that has been reduced by transparency
  //needs to be a real pointer to support polymorphism
  virtual Light * transparent(double transparency) = 0;

  virtual std::istream& in(std::istream&) = 0;
  virtual std::ostream& out(std::ostream&) const = 0;
  
  Point3Dd ambient;
  Point3Dd diffuse;
  Point3Dd specular;
  double specularCo;

 protected:
  //features we want visible to inherited classes, but not public
  double power;
};

#undef COMPILINGLIGHT
#endif
