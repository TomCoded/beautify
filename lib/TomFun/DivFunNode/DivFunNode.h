#ifndef DIVFUNNODE_H_
#define DIVFUNNODE_H_
//DivFunNode.h (C) 2002 Tom White
//does leftnode / rightnode
//returns leftnode / 0.00001 on divide by zero

#include <FunNode/FunNode.h>

class DivFunNode : public FunNode
{
 public:
  DivFunNode();
  DivFunNode(FunNode * leftNode, FunNode * rightNode);
  DivFunNode(DivFunNode &other);
  DivFunNode * clone();
  ~DivFunNode();

  double eval(double t);
  double eval(int date, Stock *s);

  ostream& out(ostream&);

 protected:
  FunNode * leftNode;
  FunNode * rightNode;
};

#endif
