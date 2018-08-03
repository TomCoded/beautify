#ifndef NUMFUNNODE_H_
#define NUMFUNNODE_H_
//NumFunNode.h (C) 2002 Tom White
//holds the final number node, for a double

#include <FunNode/FunNode.h>

class NumFunNode : public FunNode
{
 public:
  NumFunNode();
  NumFunNode(double myVal);
  NumFunNode(NumFunNode &other);
  NumFunNode * clone();
  ~NumFunNode();

  double eval(double t);

  std::ostream& out(std::ostream&);

 protected:
  double myVal;
};

#endif
