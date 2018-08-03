// This may look like C code, but it is really -*- C++ -*-

// Point3DfTest.cc

// (C) 2002 Bill Lenhart

#include "Point3Df/Point3Df.h"
#include "Transform3Df/Transform3Df.h"

void print(const char * string, const Point3Df& p)
{
  std::cout << string << " = " << p <<std::endl;
}

void print(const char * string, const Transform3Df& t)
{
  std::cout << string << " = " << t <<std::endl;
}

int main(void) {

  Point3Df p1, p2(1,2,3), p;
  Point3Df q(2.3,2.3,2.3);

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
  print("p1 x (1,1,0)",p1.cross(Point3Df(1,1,0)));

  p = (p1 * 3) - (p2 * 2);
  print("p ",p);

  print("p1 * p2 ",p1 * p2);
  std::cout << "p1 dot p2 = " << p1.dot(p2) <<std::endl;
  
  std::cout << p1.x << ",";
  std::cout << p1.y << ",";
  std::cout << p1.z <<std::endl;

  Transform3Df t(1,2,3,4,5,6,7,8,9);
  print("t ",t);
  print("t*p ",p.apply(t));
  print("t*p ",p.applyToSelf(t));
  p2.x = 11;
  std::cout << p2 <<std::endl;
}
