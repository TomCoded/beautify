#ifndef MULTFUNNODE_H_
#define MULTFUNNODE_H_
//MultFunNode.h (C) 2002 Tom White
//multiplies two nodes

#include <FunNode/FunNode.h>

class MultFunNode : public FunNode
{
 public:
  MultFunNode();
  MultFunNode(FunNode * leftNode, FunNode * rightNode);
  MultFunNode(MultFunNode &other);
  MultFunNode * clone();
  ~MultFunNode();

  ostream& out(ostream&);

  double eval(double t);
 protected:
  FunNode * leftNode;
  FunNode * rightNode;
};

#endif
