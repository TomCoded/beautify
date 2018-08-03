#ifndef FUNNODE_H_
#define FUNNODE_H_
//FunNode.h (C) 2002 Tom White
//Holds interface for all function classes

#include <FunDefs.h>
#include <iostream.h>
#include <string>

class FunNode
{
 public:
  FunNode();
  FunNode(FunNode &other);
  ~FunNode();

  virtual FunNode * clone(void) {};
  FunNode * parseString(string szFunction);

  //assignment operator
  FunNode * operator=(FunNode& other);
  
  //add, sub, multiply, divide
  FunNode * operator +(FunNode*);
  FunNode * operator -(FunNode*);
  FunNode * operator *(FunNode*);
  FunNode * operator /(FunNode*);
  
  FunNode * operator +(const double scalar);
  FunNode * operator -(const double scalar);
  FunNode * operator *(const double scalar);
  FunNode * operator /(const double scalar);

  virtual double eval(double t)=0;
  
  FunNode * in(istream& is);
  virtual ostream& out(ostream&);

 protected:
};

istream& operator>>(istream& is, FunNode&);
ostream& operator<<(ostream& o, FunNode&);

#endif
