#ifndef PHOTON_H_
#define PHOTON_H_

#include <iostream>
#include <Point3Dd.h>
//#include <Ray/Ray.h>

struct Photon {
  float x,y,z; //location of photon
  float r,g,b; //intensity of photon
  float dx, dy, dz; //incident direction of photon
  int offset; //flag used to synchronize parallel computation
  short flag; //flag used by kdTree
};

std::ostream& operator<<(std::ostream&, Photon&); 
std::istream& operator>>(std::istream&, Photon&);

#endif
