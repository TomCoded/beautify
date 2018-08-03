// This may look like C code, but it is really -*- C++ -*-

// Plane3DTest.cc

// (C) 2002 Bill Lenhart

#include "Plane/Plane.h"

extern "C" {
#include <math.h>
}

int main(void) {
  Plane* p1 = new Plane();
  Shape* p2 = new Plane(Point3Dd(1,1,2), Point3Dd(1,0,1));

  cout << "p1 = ";
  p1->out(cout) << endl;
  cout << "p2 = " << " ";
  p2->out(cout) << endl;

  Shape* p3 = p1->create();
  Shape* p4 = p2->clone();
  cout << "p3 = ";
  p3->out(cout) << endl;
  cout << "p4 = " << " ";
  p4->out(cout) << endl;

  Ray r1(Point3Dd(0,0,2),Point3Dd(0,0,-1)),
    r2(Point3Dd(-0.2,-0.2,4),Point3Dd(-0.15,-0.1,-1));
  cout << "r1 = " << r1 << endl << "r2 = " << " " << r2 << endl;

  double t = p1->closestIntersect(r1);

  if(t<0)
    cout << "t = " << t << ": "
	 << "Ray r1 does not intersect plane p1" << endl;
  else
    cout << "Ray r1 intersects plane p1 when t = " << t
	 << " at point " << r1.GetPointAt(t) << endl;

  t = p1->closestIntersect(r2);
  if(t<0)
    cout << "t = " << t << ": "
	 << "Ray r2 does not intersect plane p1" << endl;
  else
    cout << "Ray r2 intersects plane p1 when t = " << t
	 << " at point " << r2.GetPointAt(t) << endl;

  t = p2->closestIntersect(r1);
  if(t<0)
    cout << "t = " << t << ": "
	 << "Ray r1 does not intersect plane p2" << endl;
  else
    cout << "Ray r1 intersects plane p2 when t = " << t
	 << " at point " << r1.GetPointAt(t) << endl;

  t = p2->closestIntersect(r2);
  if(t<0)
    cout << "t = " << t << ": "
	 << "Ray r2 does not intersect plane p2" << endl;
  else
    cout << "Ray r2 intersects plane p2 when t = " << t
	 << " at point " << r2.GetPointAt(t) << endl;

}
