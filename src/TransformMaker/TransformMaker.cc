// This may look like C code, but it is really -*- C++ -*-

// TransformMaker.cc

// Some useful matrix generators

// (C) Anonymous1 2002

#include "TransformMaker/TransformMaker.h"

// All angles are in radians:  2*PI radians = 360 degrees

// Return an array, in column major order, holding entries of transform
double * GetArray(const Transform4Dd& tr) {
  double *m = new double[16];
  int i=0;
  for(int c=0; c<4;c++)
    for(int r=0; r<4;r++)
      m[i++] = tr.data[r][c];
  return m;
}

// A transform that rotates vec onto dir around (vec x dir)
Transform4Dd MakeRotation(const Point3Dd& vec, const Point3Dd& dir) {
  Point3Dd fromVec = vec;
  fromVec.normalize();
  Point3Dd toVec = dir;
  toVec.normalize();
  Point3Dd cross = fromVec.cross(toVec);
  cross.normalize();
  return MakeRotation(acos(fromVec.dot(toVec)),cross);
}

// Make a counter-clockwise rotation of angle theta about std::vector d
Transform4Dd MakeRotation(double theta, const Point3Dd& d) 
{
  double c = cos(theta);
  double s = sin(theta);
  return Transform4Dd(c+(1-c)*d.x*d.x, (1-c)*d.x*d.y-s*d.z,
		      (1-c)*d.x*d.z+s*d.y, 0,
		      (1-c)*d.x*d.y+s*d.z, c+(1-c)*d.y*d.y,
		      (1-c)*d.y*d.z-s*d.x, 0,
		      (1-c)*d.x*d.z-s*d.y, (1-c)*d.y*d.z+s*d.x,
		      c+(1-c)*d.z*d.z, 0,
		      0, 0, 0, 1);
}
  
// Make a counter-clockwise rotation of angle theta about the x axis
Transform4Dd MakeXRotation(double theta)
{
  return Transform4Dd (1,0,0,0,0,cos(theta),-sin(theta),0,
		       0,sin(theta),cos(theta),0,0,0,0,1);
}
  
// Make a counter-clockwise rotation of angle theta about the y axis
Transform4Dd MakeYRotation(double theta)
{
  return Transform4Dd (cos(theta),0,sin(theta),0,0,1,0,0,
		       -sin(theta),0,cos(theta),0,0,0,0,1);
}
  
// Make a counter-clockwise rotation of angle theta about the z axis
Transform4Dd MakeZRotation(double theta)
{
  return Transform4Dd (cos(theta),-sin(theta),0,0,
		       sin(theta),cos(theta),0,0,
		       0,0,1,0,0,0,0,1);
}

// Make a  rotation sending b1,b2,b3 (orthonormal) to x,y,z axes
Transform4Dd MakeRotation(const Point3Dd& b1,
			   const Point3Dd& b2,
			   const Point3Dd& b3)
{
  return Transform4Dd (b1.x,b1.y,b1.z,0,b2.x,b2.y,b2.z,0,
		       b3.x,b3.y,b3.z,0,0,0,0,1);
}
  
// Translate x,y and z by p.x,p.y,p.z respectively
Transform4Dd MakeTranslation(const Point3Dd& p)
{
  return Transform4Dd (1,0,0,p.x,
		       0,1,0,p.y,
		       0,0,1,p.z,
		       0,0,0,1);
}

// Translate x,y and z by dx,dy, and dz respectively
Transform4Dd MakeTranslation(double dx, double dy, double dz)
{
  return Transform4Dd (1,0,0,dx,
		       0,1,0,dy,
		       0,0,1,dz,
		       0,0,0,1);
}
  
// scale x,y and z by p.x,p.y,p.z respectively
Transform4Dd MakeScale(const Point3Dd& p)
{
  return Transform4Dd (p.x,0,0,0,
		       0,p.y,0,0,
		       0,0,p.z,0,
		       0,0,0,1);
}
  
// scale x,y and z by sx,sy, and sz respectively
Transform4Dd MakeScale(double sx, double sy, double sz)
{
  return Transform4Dd (sx,0,0,0,
		       0,sy,0,0,
		       0,0,sz,0,
		       0,0,0,1);
}
