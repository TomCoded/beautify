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
  double eval(int date, Stock *s);

  ostream& out(ostream&);

 protected:
  double myVal;
};

#endif
