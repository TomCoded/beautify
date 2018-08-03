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
      cout << "Input expression: " << endl;
      cin >> myString;
      myFun = myParser.parseString(myString);
      done=false;
      while(!done)
	{
	  cout << "Evaluating expression at t = ?: ";
	  cin >> t;
	  cout << "Value of ";
	  myFun->out(cout);
	  cout << " at t = " << t;
	  
	  cout << " is " << (myFun->eval(t)) <<
	    endl;
	  cout << "(q)uit/(e)xpression/(c)ontinue:";
	  cin >> c;
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
