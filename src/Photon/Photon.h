#ifndef PHOTON_H_
#define PHOTON_H_

#include <iostream>
#include <Point3Dd.h>
#include <Ray/Ray.h>

struct Photon {
  float x,y,z;
  float r,g,b;
  float dx, dy, dz; //incident direction of photon
  int offset; //flag used to synchronize parallel computation
  short flag; //flag used by kdTree
};

inline Photon createPhoton(Point3Dd location,
			   Point3Dd incidentDir,
			   Point3Dd intensity
			   );

std::ostream& operator<<(std::ostream&, Photon&); 
std::istream& operator>>(std::istream&, Photon&);

#endif
