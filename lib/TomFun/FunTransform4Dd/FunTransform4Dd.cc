// FunTransform4Dd.cc (C) 2002 Tom White

#include <Point4Dd.h>
#include <FunNode/FunNode.h>
#include <SumFunNode/SumFunNode.h>
#include <MultFunNode/MultFunNode.h>
#include <NumFunNode/NumFunNode.h>
#include "FunTransform4Dd.h"


// Constructors
// Default: create identity transform
FunTransform4Dd::FunTransform4Dd():
  destroyOnDeath(true)
{
  for (int r=0;r<rowSize;r++)
    for (int c=0;c<colSize;c++) {
      if(r==c)
	data[r][c] = new NumFunNode(1);
      else
	data[r][c] = new NumFunNode(0);
    }
}

// Create transform with given three columns
FunTransform4Dd::FunTransform4Dd(const POINT_TYPE_4D& col0, const POINT_TYPE_4D& col1,
				 const POINT_TYPE_4D& col2, const
				 POINT_TYPE_4D& col3):
  destroyOnDeath(true)
{
  setCol(0,col0);
  setCol(1,col1);
  setCol(2,col2);
  setCol(3,col3);
}

// Create transform with given values (Row-major order!)
FunTransform4Dd::FunTransform4Dd(double r0c0, double r0c1, double r0c2, double r0c3,
				 double r1c0, double r1c1, double r1c2, double r1c3,
				 double r2c0, double r2c1, double r2c2, double r2c3,
				 double r3c0, double r3c1, double
				 r3c2, double r3c3):
  destroyOnDeath(true)
{
  data[0][0] = new NumFunNode(r0c0);
  data[0][1] = new NumFunNode(r0c1);
  data[0][2] = new NumFunNode(r0c2);
  data[0][3] = new NumFunNode(r0c3);
  data[1][0] = new NumFunNode(r1c0);
  data[1][1] = new NumFunNode(r1c1);
  data[1][2] = new NumFunNode(r1c2);
  data[1][3] = new NumFunNode(r1c3);
  data[2][0] = new NumFunNode(r2c0);
  data[2][1] = new NumFunNode(r2c1);
  data[2][2] = new NumFunNode(r2c2);
  data[2][3] = new NumFunNode(r2c3);
  data[3][0] = new NumFunNode(r3c0);
  data[3][1] = new NumFunNode(r3c1);
  data[3][2] = new NumFunNode(r3c2);
  data[3][3] = new NumFunNode(r3c3);
};
  
// copy constructor
FunTransform4Dd::FunTransform4Dd(const FunTransform4Dd& other)
{
  *this = other;
}

// assignment operator;
FunTransform4Dd& FunTransform4Dd::operator=(const FunTransform4Dd& other)
{
  if (this != &other) {
    for(int r=0;r<rowSize;r++)
      for (int c=0;c<colSize;c++)
	{
	  if(destroyOnDeath) {
	    delete data[r][c];
	  }
	  data[r][c] = other.data[r][c]->clone();
	  //	  cerr << "Assigning " << r << ',' << c << ':';
	  //	  data[r][c]->out(cerr);
	  //	  cerr << endl;
	}
    destroyOnDeath=true;
  }
  return *this;
}

FunTransform4Dd::~FunTransform4Dd() 
{
  if(destroyOnDeath)
    for(int r=0; r<rowSize;r++)
      for(int c=0; c<colSize; c++)
	delete data[r][c];
}

//get Transform4Dd for current Time
Transform4Dd FunTransform4Dd::getStaticTransform()
{
  return Transform4Dd(data[0][0]->eval(t), data[0][1]->eval(t), 
		      data[0][2]->eval(t), data[0][3]->eval(t),
		      data[1][0]->eval(t), data[1][1]->eval(t),
		      data[1][2]->eval(t), data[1][3]->eval(t),
		      data[2][0]->eval(t), data[2][1]->eval(t),
		      data[2][2]->eval(t), data[2][3]->eval(t),
		      data[3][0]->eval(t), data[3][1]->eval(t),
		      data[3][2]->eval(t), data[3][3]->eval(t));
}

// access row r, column c
FunNode * FunTransform4Dd::entry(int row,int col) const
{
  if ((0 <= row) && (row < rowSize) && (0 <= col) && (col <= colSize))
    return data[row][col];
  else {
    cout << "Range error in function entry()" << endl;
    exit(1);
  }
}

// access row r, column c
FunNode*& FunTransform4Dd::entry(int row,int col)
{
  if ((0 <= row) && (row < rowSize) && (0 <= col) && (col <= colSize))
    return data[row][col];
  else {
    cout << "Range error in function entry()" << endl;
    exit(1);
  }
}

// copy and return row r or column c as a POINT_TYPE_4D
/*
POINT_TYPE_4D FunTransform4Dd::getRow(int r) const
{
  POINT_TYPE_4D temp;
 
  for(int c=0;c<colSize;c++)
    temp.data[c] = data[r][c];
  
  return temp;
}

POINT_TYPE_4D FunTransform4Dd::getCol(int c) const
{
  POINT_TYPE_4D temp;

  for(int r=0;r<rowSize;r++)
    temp.data[r] = data[r][c];
  
  return temp;
}
*/

// replace row r or column c with POINT_TYPE_4D
void FunTransform4Dd::setRow(int r,const POINT_TYPE_4D& row)
{
  for(int c=0;c<colSize;c++)
    {
      delete data[r][c];
      data[r][c] = new NumFunNode(row.data[c]);
    }
}

void FunTransform4Dd::setCol(int c,const POINT_TYPE_4D& col)
{
  for(int r=0;r<rowSize;r++)
    {
      delete data[r][c];
      data[r][c] = new NumFunNode(col.data[r]);
    }
}

// Matrix operations

// perform transpose on Transform
FunTransform4Dd& FunTransform4Dd::transposeSelf(void)
{
  for(int r=0;r<rowSize-1;r++)
    for(int c=r+1;c < colSize;c++) {
      FunNode * save = data[c][r];
      data[c][r] = data[r][c];
      data[r][c] = save;
    }
  return *this;
}

// compute transpose of Transform
FunTransform4Dd FunTransform4Dd::transpose(void) const
{
  FunTransform4Dd temp;

  for(int r=0;r<rowSize;r++)
    for(int c=0;c < colSize;c++)
      temp.data[c][r] = data[r][c]->clone();
  return temp;
}

// increment/decrement/multiply assignment operators
FunTransform4Dd& FunTransform4Dd::operator+=(const FunTransform4Dd& other)
{
  if((rowSize == other.rowSize) && (colSize == other.colSize)) {
    for(int r=0;r<rowSize;r++)
      for(int c=0;c<colSize;c++)
	data[r][c] = new SumFunNode(data[r][c],
				    other.data[r][c]->clone());
	//	data[r][c] += other.data[r][c];
    return *this;
  }
  else
    cerr << "Matrix addition error: different shapes" << endl;
}

FunTransform4Dd& FunTransform4Dd::operator-=(const FunTransform4Dd& other)
{
  if((rowSize == other.rowSize) && (colSize == other.colSize)) {
    for(int r=0;r<rowSize;r++)
      for(int c=0;c<colSize;c++)
	data[r][c] = new SumFunNode(data[r][c], new MultFunNode(new
								NumFunNode(-1), other.data[r][c]->clone()));
	//	data[r][c] -= other.data[r][c];
    return *this;
  }
  else
    cerr << "Matrix subtraction error: different shapes" << endl;
}

// note: this is matrix multiplication--NOT co-ordinate-wise
FunTransform4Dd& FunTransform4Dd::operator*=(const FunTransform4Dd& other)
{
  //get pointers to all nodes that will be needed in the new matrix
  //current elts of this matrix
  FunNode * olddata[rowSize][colSize];
  //clones of elts of other matrix
  FunNode * othermat[other.rowSize][other.colSize];
  FunNode * temp;

  if(colSize==other.rowSize) {

  for(int i=0; i<rowSize; i++) 
    for(int j=0; j<colSize; j++) {
      //clear current matrix for new entries
      olddata[i][j]=data[i][j];
      data[i][j]=0;
    }

  if(this == &other) {
    for(int i=0; i<rowSize; i++)
      for(int j=0; j<colSize; j++)
	othermat[i][j]=olddata[i][j]->clone();
  } 
  else 
    for(int i=0; i<other.rowSize; i++)
      for(int j=0; j<other.colSize; j++)
	//clone other matrix
	othermat[i][j]=(other.data[i][j]->clone());
  
  for(int i=0; i<rowSize; i++)
    for(int j=0; j<other.colSize; j++) {
      for(int k=0; k<rowSize; k++) {
	//create partial sum
	  temp=
	    new MultFunNode(olddata[i][k],
			    othermat[k][j]);

	  //if we're not at the end of a row,
	  //we'll need a funNode again in the
	  //next iteration.  Clone it.
	  if(k!=rowSize) {
	    othermat[k][j]=
	      othermat[k][j]->clone();
	    olddata[i][k]=
	      olddata[i][k]->clone();
	  }

	  //load partial sum into data[i][j]
	  if(!data[i][j]) {
	    data[i][j]=temp;
	  } else {
	    data[i][j]=
	      new SumFunNode(data[i][j],
			     temp);
	  }
      }
    }
  
  }
  else {
   cerr << "Matrix product error: different shapes" << endl;
   exit(1);
 }
}

// increment/decrement/multiply self by scalar
FunTransform4Dd& FunTransform4Dd::operator+=(const double scalar)
{
  for(int r=0; r<rowSize; r++)
    for(int c=0;c<colSize;c++)
      data[r][c] = new MultFunNode(new NumFunNode(scalar),data[r][c]);
  
  return *this;
}

FunTransform4Dd& FunTransform4Dd::operator-=(const double scalar)
{
  for(int r=0; r<rowSize; r++)
    for(int c=0; c<colSize; c++)
      data[r][c] 
	= new SumFunNode(
			 data[r][c],
			 new MultFunNode(
					 new NumFunNode(-1),
					 new NumFunNode(scalar)
					 )
			 );
  
  return *this;
}

FunTransform4Dd& FunTransform4Dd::operator*=(const double scalar)
{
  for(int r=0;r<rowSize;r++)
    for(int c=0;c<colSize;c++)
      data[r][c] = 
	new MultFunNode(data[r][c],new NumFunNode(scalar));
  
  return *this;
}

// overloaded binary operators

// matrix addition, subtraction and multiplication
FunTransform4Dd FunTransform4Dd::operator+(const FunTransform4Dd& m2) const
{
  FunTransform4Dd temp;
  temp = *this;
  temp += m2;
  return temp;
}

FunTransform4Dd FunTransform4Dd::operator-(const FunTransform4Dd& m2) const
{
  FunTransform4Dd temp;
  temp = *this;
  temp -= m2;
  return temp;
}

// note: this is matrix multiplication--NOT co-ordinate-wise
FunTransform4Dd FunTransform4Dd::operator*(const FunTransform4Dd& m2) const
{
  FunTransform4Dd temp;
  temp = *this;
  temp *= m2;
  return temp;
}

// operators for addition, subtraction, multiplication by a scalar
// always put matrix on LEFdouble side
FunTransform4Dd FunTransform4Dd::operator+(const double x) const
{
  FunTransform4Dd temp;

  temp = *this;
  temp += x;
  return temp;
}

FunTransform4Dd FunTransform4Dd::operator-(const double x) const
{
  FunTransform4Dd temp;

  temp = *this;
  temp -= x;
  return temp;
}

FunTransform4Dd FunTransform4Dd::operator*(const double x) const
{
  FunTransform4Dd temp;

  temp = *this;
  temp *= x;
  return temp;
}

// Apply matrix to POINT_TYPE_4D p
POINT_TYPE_4D FunTransform4Dd::operator*(const POINT_TYPE_4D& v) const
{
  POINT_TYPE_4D result(0,0,0,0);
  double nowArray[rowSize][colSize];

  for(int r=0; r<rowSize; r++)
    for(int c=0; c<colSize; c++)
      nowArray[r][c] = data[r][c]->eval(t);

  for(int r=0;r<rowSize;r++){
    for(int c=0;c<colSize;c++)
      result.data[r] += nowArray[r][c]*v.data[c];
  }
  return result;
}

// append transform to stream
ostream& FunTransform4Dd::out(ostream& o) const
{
  o << "(";
  for(int r=0;r<rowSize;r++) {
    o << "(";
    for(int c=0;c<colSize;c++) 
      {
        data[r][c]->out(o);
	o << ' ';
      }
    o << ")";
  }
  return o;
}

// read Transform from stream
istream& FunTransform4Dd::in(istream& is)
{
  char c;
  POINT_TYPE_4D input;
  SumFunNode parser;

  for(int r=0; r<rowSize; r++)
    {
      for(int c=0; c<colSize; c++)
	{
	  data[r][c]=parser.in(is);
	}
    }

#if 0  
  cerr << "Read fun transform:\n";
  for(int i=0; i<4; i++)
    for(int j=0; j<4; j++)
      {
	data[i][j]->out(cerr);
	cerr << ' ';
      }
  cerr << endl;
#endif

  return is;
}

// read transform from stream
istream& operator>>(istream& is, FunTransform4Dd& t)
{
  t.in(is);
}

// append transform to stream
ostream& operator<<(ostream& o, const FunTransform4Dd& t)
{
  return t.out(o);
}





