/* (C) 2002 Tom White -*- C++ -*-
   A Diffuse point light emits photons in uniformly distributed
   random directions from a point.
*/

#ifndef DIFFUSEPOINTLIGHT_H_
#define DIFFUSEPOINTLIGHT_H_

#include <Point3Dd.h>
#include <PhotonMap/PhotonMap.h>
#include <Light/PointLight/PointLight.h>

class DiffusePointLight : public PointLight
{
 public:
  DiffusePointLight();
  DiffusePointLight(DiffusePointLight&);
  ~DiffusePointLight();

  void addPhotonsToMap(int numPhotons,
		       PhotonMap *,
		       Renderer * Renderer
		       );

  Point3Dd getPower();

  std::istream& in(std::istream&);
  std::ostream& out(std::ostream&);

 protected:
};

std::istream& operator>>(std::istream&, DiffusePointLight&);
std::ostream& operator<<(std::ostream&, DiffusePointLight&);

#endif
