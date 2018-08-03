#include "CosFunNode.h"
//CosFunNode.h (C) 2002 Tom White
//does cos(childNode)

#include <math.h>

CosFunNode::CosFunNode():
  childNode(0)
{}

//destroys on exit
CosFunNode::CosFunNode(FunNode * childNode):
  childNode(childNode)
{}

CLONEMETHOD(CosFunNode)

CosFunNode::CosFunNode(CosFunNode &other)
{
  if(this!=&other)
    {
      if(childNode)
	delete childNode;
      childNode=other.childNode->clone();
    }
}

CosFunNode::~CosFunNode() 
{
  if(childNode)
    delete childNode;
}

double CosFunNode::eval(double t) 
{
  return cos(childNode->eval(t));
}

double CosFunNode::eval(int day, Stock *s) {
  return cos(childNode->eval(day,s));
}

ostream & CosFunNode::out(ostream& o)
{
  o << "cos(";
  childNode->out(o);
  o << ')';
  return o;
}
