/* -*- C++ -*-
   Material.cc (C)  Tom White
   Includes class declaration for the Material Class.
   This class holds material information about a surface,
   and contains the implementation of Russian Roulette.
*/

#include "Material.h"
#include <string>
#include <stdlib.h>
#include "Defs.h"

Material::Material():
  ambient(0.5,0.5,0.5),
  diffuse(0.8,0.8,0.8),
  specular(0.2,0.2,0.2),
  specCo(0.2)
{}

Material::Material(Material &other)
{
  if(this!=&other)
    {
      ambient=other.ambient;
      specular=other.specular;
      diffuse=other.diffuse;
      specCo=other.specCo;
    }
}

CLONEMETHOD(Material)

Material::~Material()
{}

bool Material::participates() const {
  return false;
}

inline double Material::max(double r, double g, double b)
{
  if(r>g)
    {
      if(g>b)
	{
	  return r;
	}
      else
	{ //r>g, b>g
	  if(r>b)
	    {
	      return r;
	    }
	  else return b;
	}
    }
  else if(g>b)
    { //g>r, g>b
      return g;
    }
  //g>r, b>g
  return b;
}

//THIS ALSO UPDATES THE POWER OF THE PHOTON
//ACCORDING TO THE PROBABILITY OF ITS SURVIVAL
// IFF it is reflected.
int Material::nRoulette(Photon &p)
{
  double dRand = drand48();
  double maxP = max(p.r,p.g,p.b);
  double probDiffReflection = max(diffuse.x*p.r,
				  diffuse.y*p.g,
				  diffuse.z*p.b) / maxP;
  double probSpecReflection = max(specular.x*p.r,
				  specular.y*p.g,
				  specular.z*p.b) / maxP;
  if(probDiffReflection + probSpecReflection >= 1)
    {
      double tempTotalProbability=
	probSpecReflection+probDiffReflection;
      probDiffReflection = probDiffReflection / 
	tempTotalProbability;
      probSpecReflection = probSpecReflection /
	tempTotalProbability;
    }
  if(dRand < probDiffReflection)
    {
      p.r = p.r*diffuse.x / probDiffReflection;
      p.g = p.g*diffuse.y / probDiffReflection;
      p.b = p.b*diffuse.z / probDiffReflection;
      return DIFFUSE_REFLECTION;
    }
  else if (dRand < (probDiffReflection+probSpecReflection))
    {
      p.r = p.r*diffuse.x / probSpecReflection;
      p.g = p.g*diffuse.y / probSpecReflection;
      p.b = p.b*diffuse.z / probSpecReflection;
      return SPECULAR_REFLECTION;
    }
  return ABSORPTION;
}

void Material::DoBRDF(Photon &p)
{
  p.r = p.r * diffuse.x;
  p.g = p.g * diffuse.y;
  p.b = p.b * diffuse.z;
}

Point3Dd Material::DoSpecBRDF(Point3Dd& incident,
			      Point3Dd& reflected
			      )
{
  return Point3Dd(specular.x,specular.y,specular.z);
}

double Material::getSpecCo(int nLightingModel)
{ //currently doesn't check phong v/ halfway
  return specCo;
}

std::istream& Material::in(std::istream &in)
{
  char c;
  std::string formatErr("Bad format for Material");

  in >> c >> ambient;
  FORMATTEST(c,'(',formatErr)
  in >> c >> diffuse;
  FORMATTEST(c,',',formatErr)
  in >> c >> specular;
  FORMATTEST(c,',',formatErr)
  in >> c >> specCo;
  FORMATTEST(c,',',formatErr)
  in >> c;
  FORMATTEST(c,')',formatErr)

  return in;
}

std::ostream& Material::out(std::ostream &o)
{
  o << '(';
  o << ambient << ','
    << diffuse << ','
    << specular << ','
    << specCo << ')';
  return o;
}

std::istream& operator>>(std::istream& is, Material &m)
{
  return m.in(is);
}

std::ostream& operator<<(std::ostream& o, Material &m)
{
  return m.out(o);
}

Point3Dd Material::getAmbient() const {
  return ambient;
}
Point3Dd Material::getDiffuse() const {
  return diffuse;
}
Point3Dd Material::getSpecular() const {
  return specular;
}
double Material::getSpecCo() const {
  return specCo;
}

