#ifndef PHOTON_H_
#define PHOTON_H_

#include <iostream>
#include <Point3Dd.h>
//#include <Ray/Ray.h>

struct Photon {
  float x,y,z; //location of photon
  float r,g,b; //intensity of photon
  float dx, dy, dz; //incident direction of photon
  short flag; //flag used by kdTree
  short bounced; //true iff has bounced once. (Used if only storing indirect illumination)
  int offset; //flag used to synchronize parallel computation
};

std::ostream& operator<<(std::ostream&, Photon&); 
std::istream& operator>>(std::istream&, Photon&);

#endif
