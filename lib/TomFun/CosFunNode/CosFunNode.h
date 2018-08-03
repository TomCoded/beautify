#ifndef COSFUNNODE_H_
#define COSFUNNODE_H_
//CosFunNode.h (C) 2002 Tom White
//returns cos(childNode)

#include <FunNode/FunNode.h>

class CosFunNode : public FunNode
{
 public:
  CosFunNode();
  CosFunNode(FunNode * childNode);
  CosFunNode(CosFunNode &other);
  CosFunNode * clone();
  ~CosFunNode();

  double eval(double t);

  ostream& out(ostream&);

 protected:
  FunNode * childNode;
};

#endif
