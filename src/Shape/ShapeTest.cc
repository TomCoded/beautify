// This may look like C code, but it is really -*- C++ -*-

// ShapeTest.cc

// (C) 2002 Bill Lenhart

#include "Shape/Shape.h"
#include "Sphere/Sphere.h"
#include "Plane/Plane.h"

extern "C" {
#include <math.h>
}

int main(void) {
  // create a std::list of shapes: note that dynamic allocation is necessary
  // for correct method dispatch
  Shape *shapeList[4];
  shapeList[0] = new Sphere();
  shapeList[1] = new Sphere(Point3Dd(1,1,2), 2);
  shapeList[2] = new Plane();
  shapeList[3] = new Plane(Point3Dd(1,1,2), Point3Dd(1,0,1));

  // Note that << doesn't work on shape pointers
  // Also << is not a Shape method, so we can't hope for correct dispatch
  // to derived types:  Therefore, use class method "out"
  for(int i=0;i<4;i++) {
    std::cout << "shapeList[" << i << "] = ";
    shapeList[i]->out(cout) <<std::endl;
  }

  // Testing cloning...
  Shape* copyList[4];
  for(int i=0;i<4;i++) {
    copyList[i] = shapeList[i]->clone();

    std::cout << "Original: ";
    shapeList[i]->out(cout) << ", copy: ";
    copyList[i]->out(cout) <<std::endl;
  }

  // create some rays
  Ray r1(Point3Dd(0,0,2),Point3Dd(0,0,-1)),
    r2(Point3Dd(-0.2,-0.2,4),Point3Dd(-0.15,-0.1,-1));
  double t; // for t-value of intersections

  std::cout << "r1 = " << r1 <<std::endl << "r2 = " << " " << r2 << endl;

  // test r1 against shapes
  for(int i=0;i<4;i++) {
    t = shapeList[i]->closestIntersect(r1);
    if(t<0)
      std::cout << "t = " << t << ": "
	   << "Ray r1 does not intersect plane shapeList[" << i << "]" <<std::endl;
    else
      std::cout << "Ray r1 intersects plane shapeList[" << i << "] when t = " << t
	   << " at point " << r1.GetPointAt(t) <<std::endl;
  }

  // now test r2
  for(int i=0;i<4;i++) {
    t = shapeList[i]->closestIntersect(r2);
    if(t<0)
      std::cout << "t = " << t << ": "
	   << "Ray r2 does not intersect plane shapeList[" << i << "]" <<std::endl;
    else
      std::cout << "Ray r2 intersects plane shapeList[" << i << "] when t = " << t
	   << " at point " << r2.GetPointAt(t) <<std::endl;
  }
}
