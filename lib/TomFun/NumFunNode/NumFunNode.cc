#include "NumFunNode.h"
//NumFunNode.h (C) 2002 Tom White
//holds the final function node, for a double

NumFunNode::NumFunNode() {}

NumFunNode::NumFunNode(double myVal):
  myVal(myVal)
{}

CLONEMETHOD(NumFunNode)

NumFunNode::NumFunNode(NumFunNode &other):
  myVal(other.myVal)
{}

NumFunNode::~NumFunNode() {}

double NumFunNode::eval(double t) 
{
  return myVal;
}

double NumFunNode::eval(int date, Stock*s)
{
  return myVal;
}

ostream& NumFunNode::out(ostream& o)
{
  o << myVal;
  return o;
}


