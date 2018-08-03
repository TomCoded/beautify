#include <iostream>

#include <FunNode/FunNode.h>
#include <FunParser/FunParser.h>

int main()
{
  string myString;
  double t;
  char c;
  bool done; bool done2=false;
  FunParser myParser;
  FunNode * myFun;
  while(!done2)
    {
      std::cout << "Input expression: " <<std::endl;
      std::cin >> myString;
      myFun = myParser.parseString(myString);
      done=false;
      while(!done)
	{
	  std::cout << "Evaluating expression at t = ?: ";
	  std::cin >> t;
	  std::cout << "Value of ";
	  myFun->out(cout);
	  std::cout << " at t = " << t;
	  
	  std::cout << " is " << (myFun->eval(t)) <<
	   std::endl;
	  std::cout << "(q)uit/(e)xpression/(c)ontinue:";
	  std::cin >> c;
	  if(c=='q') 
	    {
	      done=true;
	      done2=true;
	    }
	  else if(c=='e')
	    done=true;
	}
    }
  return 0;
}
