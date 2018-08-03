#include "FunNode.h"
#include <Funheaders.h>
#include <stdlib.h>

FunNode::FunNode() {}

FunNode::FunNode(FunNode &other)
{}

FunNode::~FunNode() {}

double FunNode::eval(double t) 
{
  return 0;
}

double FunNode::eval(int date, Stock * stock) {
  UNIMPLEMENTED("FunNode::eval");
  return 0;
}

//operator overloading:
  //assignment operator
  FunNode * FunNode::operator=(FunNode& other)
{
  return other.clone();
}
  
//add, sub, multiply, divide
FunNode * FunNode::operator +(FunNode * other)
{
  return new SumFunNode(clone(),other->clone());
};

FunNode * FunNode::operator -(FunNode * other)
{
  return new SumFunNode(clone(),
			new MultFunNode(new NumFunNode(-1),
					other->clone()
					)
			);
};

FunNode * FunNode::operator *(FunNode * other)
{
  return new MultFunNode(clone(),other->clone());
};

FunNode * FunNode::operator /(FunNode * other)
{
  return new DivFunNode(clone(),other->clone());
};
  
FunNode * FunNode::operator +(const double scalar)
{
  return new SumFunNode(clone(),new NumFunNode(scalar));
};

FunNode * FunNode::operator -(const double scalar)
{
  return new SumFunNode(clone(),
			new NumFunNode(-scalar)
			);
};

FunNode * FunNode::operator *(const double scalar)
{
  return new MultFunNode(clone(),
			 new NumFunNode(scalar)
			 );
};

FunNode * FunNode::operator /(const double scalar)
{
  return new DivFunNode(clone(),
			new NumFunNode(scalar));
};

FunNode * FunNode::in(std::istream& is)
{
  std::string s;
  is >> s;
  return parseString(s);
};

std::ostream& FunNode::out(std::ostream& o)
{
  o << "Function Node at " << this <<std::endl;
  return o;
};

// read Function
std::istream& operator>>(std::istream& is, FunNode& f)
{
  f.in(is);
}

// write function
std::ostream& operator<<(std::ostream& o,  FunNode& f)
{
  return f.out(o);
}

FunNode * FunNode::parseString(std::string szFunction)
{ //doesn't support - at beginning of string
  int nLen = szFunction.length();
  int nPlace=0;
  FunNode * leftNode;
  FunNode * rightNode;
  if(szFunction[0]=='(') //we need to evaluate a parenthesized
			 //expression
    {
      int nLast;
      for(nLast=0;szFunction[nLast]!=')';nLast++);
      //nLast is now ')' location
      return parseString(szFunction.substr(1,szFunction.length()-1));
    }
  while(nPlace<nLen)
    { //find lowest priority operator
      if(szFunction[nPlace]=='(')
	{
	  int inParen=1;
	  nPlace++;
	  while(inParen)
	    {
	      if(szFunction[nPlace]==')')
		inParen--;
	      else if(szFunction[nPlace]=='(')
		inParen++;
	      nPlace++;
	    }
	  if(nPlace==nLen) nPlace--;
	}
      if(szFunction[nPlace]=='+')
	{ //do additions last; so they are at the root node
	  leftNode = parseString(szFunction.substr(0,nPlace));
	  rightNode = parseString(szFunction.substr(nPlace+1,nLen-nPlace));
	  return new SumFunNode(leftNode, rightNode);
	}
      else if(szFunction[nPlace]=='-')
	{ 
	  leftNode = parseString(szFunction.substr(0,nPlace));
	  rightNode =
	  parseString(szFunction.substr(nPlace+1,nLen-nPlace));
	  rightNode = new MultFunNode(new NumFunNode(-1.0),rightNode);
	  return new SumFunNode(leftNode, rightNode);
	}
      nPlace++;
    }
  // no additions or subtractions found; move on to mult/div
  nPlace=0;
  while(nPlace<nLen)
    {
      if(szFunction[nPlace]=='(')
	{
	  int inParen=1;
	  nPlace++;
	  while(inParen)
	    {
	      if(szFunction[nPlace]==')')
		inParen--;
	      if(szFunction[nPlace]=='(')
		inParen++;
	      nPlace++;
	    }
	  if(nPlace==nLen) nPlace--;
	}
      if(szFunction[nPlace]=='*')
	{
	  leftNode = parseString(szFunction.substr(0,nPlace));
	  rightNode = parseString(szFunction.substr(nPlace+1,nLen-nPlace));
	  return new MultFunNode(leftNode, rightNode);
	}
      else if(szFunction[nPlace]=='/')
	{
	  leftNode = parseString(szFunction.substr(0,nPlace));
	  rightNode = parseString(szFunction.substr(nPlace+1,nLen-nPlace));
	  return new DivFunNode(leftNode, rightNode);
	}
      nPlace++;
    }
  //multiplication, division not found.  Move on to powers
  for(nPlace=0; nPlace<nLen; nPlace++)
    {
      if(szFunction[nPlace]=='(')
	{
	  int inParen=1;
	  nPlace++;
	  while((inParen)&&(nPlace<nLen))
	    {
	      if(szFunction[nPlace]==')')
		inParen--;
	      if(szFunction[nPlace]=='(')
		inParen++;
	      nPlace++;
	    }
	  if(nPlace==nLen) nPlace--;
	}
      if(szFunction[nPlace]=='^')
	{
	  leftNode = parseString(szFunction.substr(0,nPlace));
	  rightNode = parseString(szFunction.substr(nPlace+1,nLen-nPlace));
	  return new PowFunNode(leftNode, rightNode);
	}
    }
  //powers not found, move onto sin & cosine
  for(nPlace=0; nPlace < nLen; nPlace++)
    {
      if(szFunction[nPlace]=='(')
	{
	  int inParen=1;
	  nPlace++;
	  while(inParen)
	    {
	      if(szFunction[nPlace]=='(')
		inParen++;
	      else if(szFunction[nPlace]==')')
		inParen--;
	      nPlace++;
	    }
	}
      if(szFunction[nPlace]=='s')
	{ //sin function
	  if(szFunction[nPlace+3]!='(')
	    {
	      std::cerr << "sin function must be written sin(...)\n";
	      exit(1);
	    }
	  rightNode =
	  parseString(szFunction.substr(nPlace+3,nLen-(nPlace+3)));
	  return new SinFunNode(rightNode);
	}
      if(szFunction[nPlace]=='c')
	{ //cos function
	  if(szFunction[nPlace+3]!='(')
	    {
	      std::cerr << "cos function must be written cos(...)\n";
	      exit(1);
	    }
	  rightNode =
	  parseString(szFunction.substr(nPlace+3,nLen-(nPlace+3)));
	  return new CosFunNode(rightNode);
	}
    }
  for(nPlace=0; nPlace<nLen; nPlace++)
    {
      if(szFunction[nPlace]=='(')
	{
	  int inParen=1;
	  nPlace++;
	  while(inParen)
	    {
	      if(szFunction[nPlace]=='(')
		inParen++;
	      else if(szFunction[nPlace]==')')
		inParen--;
	      nPlace++;
	    }
	}
      if(szFunction[nPlace]=='t')
	{
	  return new VarFunNode();
	}
      else if((szFunction[nPlace]>='0')&&(szFunction[nPlace]<='9'))
	{ //is a number; read a double
	  double dVal =
	  atof(szFunction.substr(nPlace,szFunction.length()-nPlace).c_str());
	  return new NumFunNode(dVal);
	}
    }
}



