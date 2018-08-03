#include <iostream>

#include <FunNode/FunNode.h>
#include <FunParser/FunParser.h>
#include <FunTransform4Dd/FunTransform4Dd.h>

int main()
{
  string myString;
  double t;
  char c;
  bool done; bool done2=false;
  FunParser myParser;
  FunTransform4Dd * myFun = new FunTransform4Dd;
  FunTransform4Dd * myFun2 = new FunTransform4Dd;
  //  string s1("1+sin(t) 0 0 0 0 1 0 0 0 0 0 1 0 0 0 0 1");
  //  string s2("2 0 0 0 0 1 0 0 0 0 0 1 0 0 0 0 1");
  //  myFun = myParser.parseString(s1);
  //  myFun2 = myParser.parseString(s2);
  std::cout << "Enter transform 1:\n";
  std::cin >> *myFun;
  std::cout << "Enter transform 2:\n";
  std::cin >> *myFun2;
  std::cout << "myFun = " << myFun <<std::endl;
  std::cout << "myFun2 = " << myFun2 <<std::endl;
  std::cout << "self-multiplying 1 by 2\n";
  (*myFun)*=(*myFun2);
  std::cout << "myFun * myFun2 = " << *myFun2 <<std::endl;
  //  std::cout << "myFun * myFun2 = " << (*myFun)*(*myFun2) <<std::endl;
  return 0;
}
