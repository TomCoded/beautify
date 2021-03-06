/* -*- C++ -*-
   Material.h (C) Tom White
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

  std::istream& in(std::istream &);
  std::ostream& out(std::ostream &);

  Point3Dd getAmbient() const;
  Point3Dd getDiffuse() const;
  Point3Dd getSpecular() const;
  double getSpecCo() const;
  
 protected:
  Point3Dd ambient;
  Point3Dd specular;
  Point3Dd diffuse;
  double specCo;

  inline double max(double r, double g, double b); //returns the
						   //largest value of
						   //three doubles
};

std::istream& operator>>(std::istream&, Material&);
std::ostream& operator<<(std::ostream&, Material&);

#endif
