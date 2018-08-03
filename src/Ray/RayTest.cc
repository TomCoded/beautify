// This may look like C code, but it is really -*- C++ -*-

// RayTest.cc

// (C) 2002 Bill Lenhart

#include "Ray/Ray.h"
#include "Point3Dd.h"

extern "C" {
#include <math.h>
}

int main(void) {
  Ray r1;
  Ray r2(Point4Dd(1,1,2,1),Point4Dd(1,-2,1,0));

  std::cout << "r1 = " << r1 <<std::endl;
  std::cout << "r2 = " << r2 <<std::endl;

  std::cout << "t-value of closest point on line through r1 to point (1,1,1) is ";
  std::cout << r1.TofClosestPoint(Point4Dd(1,1,1,1)) <<std::endl;

  std::cout << "Closest point on line through r1 to point (1,1,1) is ";
  std::cout << r1.ClosestPoint(Point4Dd(1,1,1,1)) <<std::endl;

  std::cout << "Point on ray r2 having t = 3 is ";
  std::cout << r2.GetPointAt(3) <<std::endl;

  std::cout << "Closest point on line through r2 to point (6,-2,3) is ";
  std::cout << r2.ClosestPoint(Point4Dd(6,-2,3,1)) <<
    " (Hint: answer should be (3,-3,4)" <<std::endl;
  Point4Dd p(6,-2,3,1);
  Point4Dd q = r2.ClosestPoint(p);
  std::cout << q <<std::endl;
}
