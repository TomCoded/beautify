#include "PowFunNode.h"
//PowFunNode.h (C) 2002 Tom White
//holds the exponent function node

#include <math.h>

PowFunNode::PowFunNode():
  leftNode(0),
  rightNode(0)
{}

//destroys on exit
PowFunNode::PowFunNode(FunNode * leftNode, FunNode * rightNode):
  leftNode(leftNode),
  rightNode(rightNode)
{}

//using this guy is a bad idea
PowFunNode::PowFunNode(PowFunNode &other):
  leftNode(0),
  rightNode(0)
{
  if(this!=&other)
    {
      if(leftNode)
	{
	  delete leftNode;
	}
      if(rightNode)
	{
	  delete rightNode;
	}
      leftNode=other.leftNode;
      rightNode=other.rightNode;
    }
}

PowFunNode::~PowFunNode() 
{
  if(leftNode)
    delete leftNode;
  if(rightNode)
    delete rightNode;
}

double PowFunNode::eval(double t) 
{
  return pow(leftNode->eval(t),rightNode->eval(t));
}

double PowFunNode::eval(int day, Stock *s) {
  return pow(leftNode->eval(day,s),rightNode->eval(day,s));
}

std::ostream& PowFunNode::out(std::ostream& o)
{
  leftNode->out(o);
  o << '^';
  rightNode->out(o);
}
