#ifndef PARTICIPATING_H_
#define PARTICIPATING_H_

//participating media class declaration

#include <Material/Material.h>
#include <iostream>

class Participating : public Material
{
 public:
  Participating();
  Participating(Participating &other);
  Participating * clone();
  ~Participating();
  
  istream& in(istream&);
  ostream& out(ostream&);

  int nRoulette(Photon &p);
  void DoBRDF(Photon &p);

 protected:
  double scatCo; //scatterring coefficient
};

#endif
