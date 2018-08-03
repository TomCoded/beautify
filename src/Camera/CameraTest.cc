// This may look like C code, but it is really -*- C++ -*-

// CameraTest.cc

// (C) 2002 Anonymous1

#include <cmath>
#include "Camera.h"

int main() {
  Camera c1;
  cout << c1 << endl;
  Camera c2(c1);
  cout << c2 << endl;
  Camera c3(Point4Dd(1,0,0,1), Point4Dd(0,0,0,1),Point4Dd(0,0,1,0),
				   asin(1), 1, 10.0, 20.0);
  cout << c3 << endl;
  cout << c3.cameraToWorld << endl;
  c3.setSampleDims(20,15);
  for(int r=0;r<20;r++)
    for(int c=0;c<15;c++)
      cout << "Ray = " <<  c3.getRay(r,c) << endl;
  return 0;
}
