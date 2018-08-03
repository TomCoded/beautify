#include "SinFunNode.h"
//SinFunNode.h (C) 2002 Tom White
//does sin(childNode)

#include <math.h>

SinFunNode::SinFunNode():
  childNode(0)
{}

//destroys on exit
SinFunNode::SinFunNode(FunNode * childNode):
  childNode(childNode)
{}

CLONEMETHOD(SinFunNode)

//using this guy is a bad idea
SinFunNode::SinFunNode(SinFunNode &other)
{
  if(this!=&other)
    {
      if(childNode)
	delete childNode;
      childNode=other.childNode->clone();
    }
}

SinFunNode::~SinFunNode() 
{
  if(childNode)
    delete childNode;
}

double SinFunNode::eval(double t) 
{
  return sin(childNode->eval(t));
}

double SinFunNode::eval(int day, Stock *s) {
  return sin(childNode->eval(day,s));
}

ostream& SinFunNode::out(ostream& o)
{
  o << "sin(";
  childNode->out(o);
  o << ')';
  return o;
}
