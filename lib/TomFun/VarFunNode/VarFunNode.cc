#include "VarFunNode.h"
//VarFunNode.h (C) 2002 Tom White
//holds the final function node, where eval(t)=t

VarFunNode::VarFunNode() {}

CLONEMETHOD(VarFunNode)

VarFunNode::VarFunNode(VarFunNode &other) {}

VarFunNode::~VarFunNode() {}

double VarFunNode::eval(double t) 
{
  return t;
}

ostream& VarFunNode::out(ostream &o)
{
  o << 't';
  return o;
}
