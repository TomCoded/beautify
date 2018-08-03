#ifndef FUNTRANSLATENODEGL_H_
#define FUNTRANSLATENODEGL_H_

//FunTranslateNodeGL.h  -*- C++ -*-
//
//(C) 2002 Tom White
// Holds the FunTranslateNodeGL class declaration.

#include <NodeGL/NodeGL.h>
#include <MaterialNodeGL/MaterialNodeGL.h>
#include <FunParser/FunParser.h>
#include <FunNode/FunNode.h>

class FunTranslateNodeGL : public NodeGL
{
 public:
  FunTranslateNodeGL();
  FunTranslateNodeGL(FunTranslateNodeGL&);
  FunTranslateNodeGL(FunNode * xFun,
		     FunNode * yFun,
		     FunNode * zFun,
		     double velocity,
		     double start
		     );
  FunTranslateNodeGL(FunNode * xFun,
		     FunNode * yFun,
		     FunNode * zFun,
		     double velocity,
		     double start,
		     double tWrap
		     );
  ~FunTranslateNodeGL();

  void ActionGL();

  std::istream& in(std::istream&);
  std::ostream& out(std::ostream&) const;

 protected:
  FunNode * xFun;
  FunNode * yFun;
  FunNode * zFun;

  double tWrap;
  double dt; //delta t per frame
  double t; //current t
};

std::istream& operator>>(std::istream &is, FunTranslateNodeGL& p);

std::ostream& operator<<(std::ostream &o, const FunTranslateNodeGL& p);

#endif
