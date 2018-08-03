#ifndef FUNORIENTNODEGL_H_
#define FUNORIENTNODEGL_H_

//FunOrientNodeGL.h  -*- C++ -*-
//
//(C) 2002 Tom White
// Holds the FunOrientNodeGL class declaration.

#include <NodeGL/NodeGL.h>
#include <MaterialNodeGL/MaterialNodeGL.h>
#include <FunParser/FunParser.h>
#include <FunNode/FunNode.h>
#include <Point3Dd.h>

class FunOrientNodeGL : public NodeGL
{
 public:
  FunOrientNodeGL();
  FunOrientNodeGL(FunOrientNodeGL&);
  FunOrientNodeGL(Point3Dd fromAxis,
		  FunNode * xFun,
		  FunNode * yFun,
		  FunNode * zFun,
		  double velocity,
		  double start
		  );
  FunOrientNodeGL(Point3Dd fromAxis,
		  FunNode * xFun,
		  FunNode * yFun,
		  FunNode * zFun,
		  double velocity,
		  double start,
		  double tWrap
		  );
  ~FunOrientNodeGL();

  void ActionGL();

  istream& in(istream&);
  ostream& out(ostream&) const;

 protected:
  FunNode * xFun;
  FunNode * yFun;
  FunNode * zFun;
  Point3Dd fromAxis;

  double tWrap;
  double dt; //delta t per frame
  double t; //current t
};

istream& operator>>(istream &is, FunOrientNodeGL& p);

ostream& operator<<(ostream &o, const FunOrientNodeGL& p);

#endif
