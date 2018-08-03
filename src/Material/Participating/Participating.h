#ifndef PARTICIPATING_H_
#define PARTICIPATING_H_

#ifdef PI
#undef PI
#endif
#define PI 3.1415926535897932384626433832795

//participating media class declaration

#include <Material/Material.h>
#include <iostream>

#include <FunNode.h>
#include <SumFunNode.h>

class Participating : public Material
{
 public:
  Participating();
  Participating(Participating &other);
  Participating * clone();
  ~Participating();

  virtual bool participates() const;

  Point3Dd phaseFunction(const Point3Dd &in, const Point3Dd &out) const;

  std::istream& in(std::istream&);
  std::ostream& out(std::ostream&);

  int nRoulette(Photon &p);
  virtual void DoBRDF(Photon &p);

  Point3Dd getScatCo(const Point3Dd &loc) const;
  Point3Dd getAbsorbCo(const Point3Dd &loc) const;
  Point3Dd getExtinctCo(const Point3Dd &loc) const;

  Point3Dd getScatCo(const double x,
		     const double y,
		     const double z) const;
  Point3Dd getAbsorbCo(const double x,
		     const double y,
		     const double z) const;
  Point3Dd getExtinctCo(const double x,
		     const double y,
		     const double z) const;
  
  void copyScatCo(Point3Dd &outparam,
		  const double x,
		  const double y,
		  const double z) const;
  void copyExtinctCo(Point3Dd &outparam,
		  const double x,
		  const double y,
		  const double z) const;

  void copyScatCo(Point3Dd &outparam,
		  const Point3Dd &loc) const;
  void copyExtinctCo(Point3Dd &outparam,
		     const Point3Dd& loc) const;

  Point3Dd scatCo; //scatterring coefficient
  Point3Dd absorbCo; //sigma_a

 protected:
  FunNode * scatX, * scatY, * scatZ;
  FunNode * absX, * absY, *absZ;
  double greenK; //constant in Henyey-Greenstein phase function
  //for importance sampling:
  // cos theta = (2e+k-1)/(2ke-k+1)
  // e random
  //scratch variable for DoBRDF
  double left;
};

#endif
