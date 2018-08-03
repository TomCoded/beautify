#include "MultFunNode.h"
//MultFunNode.h (C) 2002 Tom White
//multiplies two nodes

MultFunNode::MultFunNode():
  leftNode(0),
  rightNode(0)
{}

//destroys on exit
MultFunNode::MultFunNode(FunNode * leftNode, FunNode * rightNode):
  leftNode(leftNode),
  rightNode(rightNode)
{}

CLONEMETHOD(MultFunNode)

MultFunNode::MultFunNode(MultFunNode &other)
{
  if(this!=&other)
    {
      leftNode=other.leftNode->clone();
      rightNode=other.rightNode->clone();
    }
}

MultFunNode::~MultFunNode() 
{
  if(leftNode)
    delete leftNode;
  if(rightNode)
    delete rightNode;
}

double MultFunNode::eval(double t) 
{
  return (leftNode->eval(t)*rightNode->eval(t));
}

double MultFunNode::eval(int date, Stock *s) {
  return (leftNode->eval(date,s)*rightNode->eval(date,s));
}

std::ostream& MultFunNode::out(std::ostream& o)
{
  leftNode->out(o);
  o << '*';
  rightNode->out(o);
  return o;
}
