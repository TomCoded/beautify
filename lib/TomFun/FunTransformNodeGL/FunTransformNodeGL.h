#ifndef FUNTRANSFORMNODEGL_H_
#define FUNTRANSFORMNODEGL_H_
//FunTransformNodeGL.h  -*- C++ -*-
//
//(C) 2002 Tom White
// Holds the FunTransformNodeGL class declaration.

#include <Point3Df.h>
#include <Transform4Dd.h>
#include <NodeGL/NodeGL.h>
#include <FunParser/FunParser.h>

class FunTransformNodeGL : public NodeGL
{
 public:
  FunTransformNodeGL();
  FunTransformNodeGL(FunTransformNodeGL&);

  ~FunTransformNodeGL();

  ostream& FunTransformNodeGL::out(ostream& o) const;
  istream& FunTransformNodeGL::in(istream& is);

  void ActionGL();

 protected:
  //array of functions, in column-major order
  FunNode ** myFuns;

  double tWrap;
  double dt;
  double t;
};

istream& operator>>(istream &is, FunTransformNodeGL& m);

ostream& operator<<(ostream &o, const FunTransformNodeGL& m);

#endif

