// This may look like C code, but it is really -*- C++ -*-

// Sphere3DTest.cc

// (C) 2002 Anonymous1

#include "Sphere/Sphere.h"

extern "C" {
#include <math.h>
}

int main(void) {
  Sphere* s1 = new Sphere();
  Shape* s2 = new Sphere(Point3Dd(1,1,2), 2);

  // bad way to output a sphere
  cout << "s1 = " << s1 << endl << "s2 = " << " " << s2 << endl;
  // good way to output a sphere
  cout << "s1 = ";
  s1->out(cout) << endl;
  cout << "s2 = " << " ";
  s2->out(cout) << endl;

  // test cloning
  Shape* s3 = s1->create();
  Shape* s4 = s2->clone();
  cout << "s3 = ";
  s3->out(cout) << endl;
  cout << "s4 = " << " ";
  s4->out(cout) << endl;

  Ray r1(Point3Dd(0,0,2),Point3Dd(0,0,-1)),
    r2(Point3Dd(-0.2,-0.2,4),Point3Dd(-0.15,-0.1,-1));

  cout << "r1 = " << r1 << endl << "r2 = " << " " << r2 << endl;

  double t = s1->closestIntersect(r1);

  if(t<0)
    cout << "t = " << t << ": "
	 << "Ray r1 does not intersect sphere s1" << endl;
  else
    cout << "Ray r1 intersects sphere s1 when t = " << t
	 << " at point " << r1.GetPointAt(t) << endl;

  t = s1->closestIntersect(r2);
  if(t<0)
    cout << "t = " << t << ": "
	 << "Ray r2 does not intersect sphere s1" << endl;
  else
    cout << "Ray r2 intersects sphere s1 when t = " << t
	 << " at point " << r1.GetPointAt(t) << endl;

  t = s2->closestIntersect(r1);
  if(t<0)
    cout << "t = " << t << ": "
	 << "Ray r1 does not intersect sphere s2" << endl;
  else
    cout << "Ray r1 intersects sphere s2 when t = " << t
	 << " at point " << r1.GetPointAt(t) << endl;

  t = s2->closestIntersect(r2);
  if(t<0)
    cout << "t = " << t << ": "
	 << "Ray r2 does not intersect sphere s2" << endl;
  else
    cout << "Ray r2 intersects sphere s2 when t = " << t
	 << " at point " << r1.GetPointAt(t) << endl;

}
