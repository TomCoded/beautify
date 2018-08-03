//FunTransformNodeGL.cc   -*- C++ -*-
//
//(C) 2002 Tom White

#include "FunTransformNodeGL.h"
#include <string.h>
#include <functions/functions.h>
#include <TransformMaker/TransformMaker.h>
#include <Transform4Dd.h>
#include <GL/glut.h>
#include <Camera/Camera.h>
#include <Scene/Scene.h>

extern Scene * g_Scene;

FunTransformNodeGL::FunTransformNodeGL():
  t(0),
  tWrap(0),
  dt(0)
{
  myFuns=new FunNode*[16]; //create array of 16 pointers to FunNodes
}

//using this guy would be a bad idea.
FunTransformNodeGL::FunTransformNodeGL(FunTransformNodeGL &other) 
{
  if(this != &other)
    {
      for(int i=0; i<16; i++)
	{
	  myFuns[i]=other.myFuns[i];
	}
    }
}

FunTransformNodeGL::~FunTransformNodeGL()
{
  for(int i=0; i<16; i++)
    {
      delete myFuns[i];
    }
}

void FunTransformNodeGL::ActionGL()
{
  glPushMatrix();
  double transform[] = 
  {
    myFuns[0]->eval(t),
    myFuns[1]->eval(t),
    myFuns[2]->eval(t),
    myFuns[3]->eval(t),
    myFuns[4]->eval(t),
    myFuns[5]->eval(t),
    myFuns[6]->eval(t),
    myFuns[7]->eval(t),
    myFuns[8]->eval(t),
    myFuns[9]->eval(t),
    myFuns[10]->eval(t),
    myFuns[11]->eval(t),
    myFuns[12]->eval(t),
    myFuns[13]->eval(t),
    myFuns[14]->eval(t),
    myFuns[15]->eval(t),
  };
  glMultMatrixd(transform);  
  t+=dt;
  if(tWrap)
    if(t>tWrap)
      t=0;
}

istream & FunTransformNodeGL::in(istream& is)
{
  string szFun;
  FunParser parser;
  for(int i=0; i<16; i++)
    {
      is >> szFun;
      myFuns[i]=parser.parseString(szFun);
    }
  is >> dt;
  is >> t;
  is >> tWrap;
  return is;
}

ostream & FunTransformNodeGL::out(ostream& o) const
{
  o << "Funtransform at address " << this << endl;
  return o;
}

istream& operator>>(istream &is, FunTransformNodeGL& m)
{
  return m.in(is);
}

ostream& operator<<(ostream &o, const FunTransformNodeGL& m)
{
  return m.out(o);
}








