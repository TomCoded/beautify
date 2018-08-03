#ifndef POWFUNNODE_H_
#define POWFUNNODE_H_
//PowFunNode.h (C) 2002 Tom White
//holds the final Function node, where eval(t)=t

#include <FunNode/FunNode.h>

class PowFunNode : public FunNode
{
 public:
  PowFunNode();
  PowFunNode(FunNode * leftNode, FunNode * rightNode);
  PowFunNode(PowFunNode &other);
  ~PowFunNode();

  double eval(double t);
  double eval(int day, Stock *s);

  ostream& out(ostream&);

 protected:
  FunNode * leftNode;
  FunNode * rightNode;
};

#endif
