#ifndef FUNPARSER_H_
#define FUNPARSER_H_

#include <FunNode/FunNode.h>
#include <NumFunNode/NumFunNode.h>
#include <VarFunNode/VarFunNode.h>
#include <SumFunNode/SumFunNode.h>
#include <MultFunNode/MultFunNode.h>
#include <DivFunNode/DivFunNode.h>
#include <PowFunNode/PowFunNode.h>
#include <SinFunNode/SinFunNode.h>
#include <CosFunNode/CosFunNode.h>
#include <string>

class FunParser
{
 public:

  FunParser();
  ~FunParser();

  FunNode * parseString(string szFunction);

  istream& in(istream&);
  ostream& out(ostream&) const;

 protected:
};

istream& operator>>(istream &is, FunParser& f);

ostream& operator<<(istream &o, const FunParser& f);

#endif
