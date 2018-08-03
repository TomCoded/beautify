// FunPoint4Dd.cc (C) 2002 Tom White

#include <Point4Dd.h>
#include <FunNode/FunNode.h>
#include <SumFunNode/SumFunNode.h>
#include <MultFunNode/MultFunNode.h>
#include <NumFunNode/NumFunNode.h>
#include "FunPoint4Dd.h"


// Constructors
// Default: create identity transform
FunPoint4Dd::FunPoint4Dd():
  destroyOnDeath(true)
{
  int c;
  for (c=0;c<size-1;c++)
    data[c] = new NumFunNode(0);

  data[c] = new NumFunNode(1);
}

// Create transform with given three columns
FunPoint4Dd::FunPoint4Dd(const POINT_TYPE_4D& col0):
  destroyOnDeath(true)
{
  data[0] = new NumFunNode(col0.PT4D_PX);
  data[1] = new NumFunNode(col0.PT4D_PY);
  data[2] = new NumFunNode(col0.PT4D_PZ);
  data[3] = new NumFunNode(col0.PT4D_PW);
}

// Create transform with given values (Row-major order!)
FunPoint4Dd::FunPoint4Dd(double c0, double c1, double c2, double c3):

  destroyOnDeath(true)
{
  data[0] = new NumFunNode(c0);
  data[1] = new NumFunNode(c1);
  data[2] = new NumFunNode(c2);
  data[3] = new NumFunNode(c3);
};
  
// copy constructor
FunPoint4Dd::FunPoint4Dd(const FunPoint4Dd& other)
{
  *this = other;
}

// assignment operator;
FunPoint4Dd& FunPoint4Dd::operator=(const FunPoint4Dd& other)
{
  if (this != &other) {
    for (int c=0;c<size;c++)
      {
	if(destroyOnDeath) {
	  delete data[c];
	}
	data[c] = other.data[c]->clone();
	//	  cerr << "Assigning " << r << ',' << c << ':';
	//	  data[r][c]->out(cerr);
	//	  cerr <<std::endl;
      }
    destroyOnDeath=true;
  }
  return *this;
}

FunPoint4Dd::~FunPoint4Dd() 
{
  if(destroyOnDeath)
      for(int c=0; c<size; c++)
	delete data[c];
}

//get Transform4Dd for current Time
POINT_TYPE_4D FunPoint4Dd::getStaticPoint()
{
  return POINT_TYPE_4D(data[0]->eval(t), data[1]->eval(t), 
		       data[2]->eval(t), data[3]->eval(t));
}

POINT_TYPE_4D FunPoint4Dd::eval(double t) {
  return POINT_TYPE_4D(data[0]->eval(t), data[1]->eval(t), 
		       data[2]->eval(t), data[3]->eval(t));
}

// Arithmatic

// increment/decrement/multiply assignment operators
FunPoint4Dd& FunPoint4Dd::operator+=(const FunPoint4Dd& other)
{
  for(int c=0;c<size;c++)
	data[c] = new SumFunNode(data[c],
				    other.data[c]->clone());
  return *this;
}

FunPoint4Dd& FunPoint4Dd::operator-=(const FunPoint4Dd& other)
{
  for(int c=0;c<size;c++)
	data[c] = new SumFunNode(data[c], new MultFunNode(new
								NumFunNode(-1), other.data[c]->clone()));
    return *this;
}

// increment/decrement/multiply self by scalar
FunPoint4Dd& FunPoint4Dd::operator+=(const double scalar)
{
  for(int c=0;c<size;c++)
      data[c] = new MultFunNode(new NumFunNode(scalar),data[c]);
  return *this;
}

FunPoint4Dd& FunPoint4Dd::operator-=(const double scalar)
{
  for(int c=0; c<size; c++)
    data[c] 
      = new SumFunNode(
		       data[c],
		       new MultFunNode(
				       new NumFunNode(-1),
				       new NumFunNode(scalar)
				       )
		       );
  
  return *this;
}

FunPoint4Dd& FunPoint4Dd::operator*=(const double scalar)
{
  for(int c=0;c<size;c++)
      data[c] = 
	new MultFunNode(data[c],new NumFunNode(scalar));
  return *this;
}

// overloaded binary operators

// addition, subtraction and multiplication
FunPoint4Dd FunPoint4Dd::operator+(const FunPoint4Dd& m2) const
{
  FunPoint4Dd temp;
  temp = *this;
  temp += m2;
  return temp;
}

FunPoint4Dd FunPoint4Dd::operator-(const FunPoint4Dd& m2) const
{
  FunPoint4Dd temp;
  temp = *this;
  temp -= m2;
  return temp;
}

// operators for addition, subtraction, multiplication by a scalar
// always put matrix on LEFdouble side
FunPoint4Dd FunPoint4Dd::operator+(const double x) const
{
  FunPoint4Dd temp;

  temp = *this;
  temp += x;
  return temp;
}

FunPoint4Dd FunPoint4Dd::operator-(const double x) const
{
  FunPoint4Dd temp;

  temp = *this;
  temp -= x;
  return temp;
}

FunPoint4Dd FunPoint4Dd::operator*(const double x) const
{
  FunPoint4Dd temp;

  temp = *this;
  temp *= x;
  return temp;
}

// append transform to stream
std::ostream& FunPoint4Dd::out(std::ostream& o) const
{
  o << "(";
  for(int c=0;c<size;c++) 
    {
      data[c]->out(o);
      o << ' ';
    }
  o << ")";
  return o;
}

// read Transform from stream
std::istream& FunPoint4Dd::in(std::istream& is)
{
  char c;
  POINT_TYPE_4D input;
  SumFunNode parser;

  for(int c=0; c<size; c++)
    {
      data[c]=parser.in(is);
    }

  return is;
}

// read transform from stream
std::istream& operator>>(std::istream& is, FunPoint4Dd& t)
{
  t.in(is);
}

// append transform to stream
std::ostream& operator<<(std::ostream& o, const FunPoint4Dd& t)
{
  return t.out(o);
}





