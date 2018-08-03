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

  istream& in(istream&);
  ostream& out(ostream&);

 protected:
};

istream& operator>>(istream&, DiffusePointLight&);
ostream& operator<<(ostream&, DiffusePointLight&);

#endif
