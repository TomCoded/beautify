// testTransform3Dd.C

// (C) 2002 Bill Lenhart

#include "Transform3Dd/Transform3Dd.h"
#include "Point3Dd/Point3Dd.h"

void print(const char * string, const Point3Dd& p)
{
  cout << string << " = " << p << endl;
}

void print(const char * string, const Transform3Dd& t)
{
  cout << string << " = " << t << endl;
}

int main(void) {

  Point3Dd p1(1,2,3), p2(4,5,6);
  Transform3Dd t1, t2(p1,p2,Point3Dd(1,1,1));

  print("p1 ",p1);
  print("p2 ",p2);

  print("t1 ",t1);
  print("t2 ",t2);

  t1 = t1 + t2;
  print("t1 = t1 + t2; t1 ",t1);

  print("t1*=t2; t1 ",t1*=t2);

  Transform3Dd t;
  print("t = ",t);

  cout << "Enter a transform: ";
  cin >> t;
  print("t = ",t);

  t = (t1 * 3) - (t2 * 2);
  print("t = (t1 * 3) - (t2 * 2); t ",t);

  print("t - t2",t-t2);

  print("t1*p2 = ", t1*p2);

  print("t2 ",t2);
  print("transpose(t2) = ",t2.transposeSelf());
}
