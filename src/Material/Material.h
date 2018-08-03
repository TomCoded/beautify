/* -*- C++ -*-
   Material.h (C) 2002 Tom White
   Includes class declaration for the Material Class.
   This class holds material information about a surface,
   and contains the implementation of Russian Roulette.
*/
#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <iostream>
#include <Point3Dd.h>
#include <Photon/Photon.h>

class Material
{
 public:
  Material();
  Material(Material&);
  Material * clone();
  ~Material();

  virtual bool participates() const;

  int nRoulette(Photon &p);
  virtual void DoBRDF(Photon &p);
  Point3Dd DoSpecBRDF(Point3Dd& incident,
		      Point3Dd& reflected
		      );
  double getSpecCo(int nLightingModel);

  istream& in(istream &);
  ostream& out(ostream &);

 protected:
  Point3Dd ambient;
  Point3Dd specular;
  Point3Dd diffuse;
  double specCo;

  inline double max(double r, double g, double b); //returns the
						   //largest value of
						   //three doubles
};

istream& operator>>(istream&, Material&);
ostream& operator<<(ostream&, Material&);

#endif
