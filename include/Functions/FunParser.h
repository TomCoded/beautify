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

  std::istream& in(std::istream&);
  std::ostream& out(std::ostream&) const;

 protected:
};

std::istream& operator>>(std::istream &is, FunParser& f);

std::ostream& operator<<(std::istream &o, const FunParser& f);

#endif
