// This may look like C code, but it is really -*- C++ -*-

// LightAtInfinityTest.cc

// (C) 1995 Bill Lenhart

#include "DirLight.h"

int main() {

  DirLight l1, l2(Point3Dd(0.3,0.4,0.3), // direction
		  Point3Df(0.4,0.5,0.1), // diffuse
		  Point3Df(0.5,0.7,0.6)); // specular

  cout << l1 << endl;
  cout << l2 << endl;
}
