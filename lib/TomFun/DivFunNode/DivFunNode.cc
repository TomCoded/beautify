#include "DivFunNode.h"
//DivFunNode.h (C) 2002 Tom White
//does leftnode / rightnode
//returns leftnode /0.00001 on div by zero

DivFunNode::DivFunNode():
  leftNode(0),
  rightNode(0)
{}

//destroys on exit
DivFunNode::DivFunNode(FunNode * leftNode, FunNode * rightNode):
  leftNode(leftNode),
  rightNode(rightNode)
{}

CLONEMETHOD(DivFunNode)

//using this guy is a bad idea
//FIXME?  Why's it bad...?
DivFunNode::DivFunNode(DivFunNode &other):
leftNode(0),rightNode(0)
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
      leftNode=other.leftNode->clone();
      rightNode=other.rightNode->clone();
    }
}

DivFunNode::~DivFunNode() 
{
  if(leftNode)
    delete leftNode;
  if(rightNode)
    delete rightNode;
}

double DivFunNode::eval(double t) 
{
  double temp = rightNode->eval(t);
  if(temp==0) temp = 0.00001;
  return (leftNode->eval(t)/temp);
}

double DivFunNode::eval(int day, Stock*s) {
  double temp = rightNode->eval(day,s);
  if(temp==0) temp=0.00001;
  return (leftNode->eval(day,s)/temp);
}

std::ostream& DivFunNode::out(std::ostream& o)
{
  leftNode->out(o);
  o << '/';
  rightNode->out(o);
}
