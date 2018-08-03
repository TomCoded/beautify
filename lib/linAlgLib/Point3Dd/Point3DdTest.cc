// This may look like C code, but it is really -*- C++ -*-

// Point3DdTest.cc

// (C) 2002 Bill Lenhart

#include "Point3Dd/Point3Dd.h"
#include "Transform3Dd/Transform3Dd.h"

void print(const char * string, const Point3Dd& p)
{
  std::cout << string << " = " << p <<std::endl;
}

void print(const char * string, const Transform3Dd& t)
{
  std::cout << string << " = " << t <<std::endl;
}

int main(void) {

  Point3Dd p1, p2(1,2,3), p;
  Point3Dd q(2.3,2.3,2.3);

  print("q ",q);
  print("p1 ",p1);
  print("p2 ",p2);
  std::cout << "norm of p2 = " << p2.norm() <<std::endl;
  print("Normalized p2 = ", p2*=1/p2.norm());
  print("Normalized p2 = ", p2.normalize());

  p1 = p1 + p2;
  p1 += p2;

  print("p1 ",p1);
  print("p2 ",p2);
  print("p1 x (1,1,0)",p1.cross(Point3Dd(1,1,0)));

  p = (p1 * 3) - (p2 * 2);
  print("p ",p);

  print("p1 * p2 ",p1 * p2);
  std::cout << "p1 dot p2 = " << p1.dot(p2) <<std::endl;
  
  std::cout << p1.x << ",";
  std::cout << p1.y << ",";
  std::cout << p1.z <<std::endl;

  Transform3Dd t(1,2,3,4,5,6,7,8,9);
  print("t ",t);
  print("t*p ",p.apply(t));
  print("t*p ",p.applyToSelf(t));
  p2.x = 11;
  std::cout << p2 <<std::endl;
}
