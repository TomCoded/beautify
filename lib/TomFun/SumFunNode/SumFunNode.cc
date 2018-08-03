#include "SumFunNode.h"
//SumFunNode.h (C) 2002 Tom White
//sums two nodes

SumFunNode::SumFunNode():
  leftNode(0),
  rightNode(0)
{}

//destroys on exit
SumFunNode::SumFunNode(FunNode * leftNode, FunNode * rightNode):
  leftNode(leftNode),
  rightNode(rightNode)
{}

SumFunNode::SumFunNode(SumFunNode &other):
  leftNode(0),
  rightNode(0)
{
  if(this!=&other)
    {
      leftNode=other.leftNode->clone();
      rightNode=other.rightNode->clone();
    }
}

FunNode * SumFunNode::clone()
{
  return new SumFunNode(*this);
}

SumFunNode::~SumFunNode() 
{
  if(leftNode)
    delete leftNode;
  if(rightNode)
    delete rightNode;
}

double SumFunNode::eval(double t) 
{
  return (leftNode->eval(t)+rightNode->eval(t));
}

double SumFunNode::eval(int date, Stock*stock) {
  return (leftNode->eval(date,stock)+rightNode->eval(date,stock));
}

std::ostream& SumFunNode::out(std::ostream& o)
{
  leftNode->out(o);
  o << '+';
  rightNode->out(o);
  return o;
}
