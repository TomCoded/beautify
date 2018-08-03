// This may look like C code, but it is really -*- C++ -*-

// Point4DdTest.cc

// (C) 2002 Bill Lenhart

#include "Point4Dd/Point4Dd.h"
#include "Transform4Dd/Transform4Dd.h"

void print(const char * string, const Point4Dd& p)
{
  cout << string << " = " << p << endl;
}

void print(const char * string, const Transform4Dd& t)
{
  cout << string << " = " << t << endl;
}

int main(void) {

  Point4Dd p1, p2(1,2,3,4), p;
  Point4Dd q(2.3,2.3,2.3,2.3);

  print("q = ",q);
  print("p1 = ",p1);
  print("p2 = ",p2);

  p1 = p1 + p2;
  p1 += p2;

  print("p1 = ",p1);
  print("p2 = ",p2);

  p = (p1 * 3) - (p2 * 2);
  print("p = ",p);

  print("p1 * p2 = ",p1 * p2);
  cout << "p1 dot p2 = " << p1.dot(p2) << endl;
  
  cout << p1.x << ",";
  cout << p1.y << ",";
  cout << p1.z << endl;

  Transform4Dd t(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
  print("t = ",t);
  print("t*p = ",p.apply(t));
  print("t*p = ",p.applyToSelf(t));
  p2.x = 11;
  cout << p2 << endl;
}
