//FunOrientNodeGL.cc   -*- C++ -*-
//
//(C) 2002 Tom White

#include "FunOrientNodeGL.h"
#include <Defs.h>
#include <GL/glut.h>
#include <math.h>

FunOrientNodeGL::FunOrientNodeGL():
  xFun(0),
  yFun(0),
  zFun(0),
  dt(0),
  t(0),
  fromAxis(0,1,0),
  tWrap(0)
{
  fromAxis.normalize();
}


//using this is a bad idea
FunOrientNodeGL::FunOrientNodeGL(FunOrientNodeGL &other) 
{
  if(this!=&other)
    {
      xFun = other.xFun;
      yFun = other.yFun;
      zFun = other.zFun;
      t=other.t;
      dt=other.dt;
      fromAxis = other.fromAxis;
      tWrap=other.tWrap;
    }
}

FunOrientNodeGL::FunOrientNodeGL(
				 Point3Dd fromAxis,
				 FunNode * xFun,
				 FunNode * yFun,
				 FunNode * zFun,
				 double velocity,
				 double start
				 ):
  xFun(xFun),
  yFun(yFun),
  zFun(zFun),
  dt(velocity),
  t(start),
  fromAxis(fromAxis),
  tWrap(0)
{
  fromAxis.normalize();
}

FunOrientNodeGL::FunOrientNodeGL(
				 Point3Dd fromAxis,
				 FunNode * xFun,
				 FunNode * yFun,
				 FunNode * zFun,
				 double velocity,
				 double start,
				 double tWrap
				 ):
  xFun(xFun),
  yFun(yFun),
  zFun(zFun),
  dt(velocity),
  t(start),
  fromAxis(fromAxis),
  tWrap(tWrap)
{
  fromAxis.normalize();
}

FunOrientNodeGL::~FunOrientNodeGL()
{
  delete xFun;
  delete yFun;
  delete zFun;
}

void FunOrientNodeGL::ActionGL()
{
  glPushMatrix();
  Point3Dd toAxis(
		  xFun->eval(t),
		  yFun->eval(t),
		  zFun->eval(t)
		  );
  toAxis.normalize();
  Point3Dd rotateAxis = fromAxis.cross(toAxis);
  rotateAxis.normalize();
  double currentAngle = acos(fromAxis.dot(toAxis));
  currentAngle = currentAngle*(180/3.14159265);
  glRotated(currentAngle,
	    rotateAxis.x,
	    rotateAxis.y,
	    rotateAxis.z
	    );
  t+=dt;
  if(tWrap)
    if(t>tWrap)
      t=0;
}

std::ostream & FunOrientNodeGL::out(std::ostream& o) const
{
  o << "FunOrient at address " << this <<std::endl;
  return o;
}

std::istream & FunOrientNodeGL::in(std::istream& is)
{
  string funX;
  string funY;
  string funZ;
  is >> fromAxis;
  is >> funX;
  is >> funY;
  is >> funZ;
  is >> dt;
  is >> t;
  is >> tWrap;
  FunParser parser;
  xFun = parser.parseString(funX);
  yFun = parser.parseString(funY);
  zFun = parser.parseString(funZ);
  return is;
}

std::istream& operator>>(std::istream &is, FunOrientNodeGL& p)
{
  return p.in(is);
}

std::ostream& operator<<(std::ostream &o, const FunOrientNodeGL& p)
{
  return p.out(o);
}
