// Transform3Dd.C

// 3x3 matrix with simple operations

// (C) 2002 Bill Lenhart

#include <stdlib.h>
#include "Point3Dd/Point3Dd.h"
#include "Transform3Dd/Transform3Dd.h"

// Constructors
// Default: create identity transform
Transform3Dd::Transform3Dd() 
{
  for (int r=0;r<rowSize;r++)
    for (int c=0;c<colSize;c++)
      if(r==c)
	data[r][c] = 1;
      else
	data[r][c] = 0;
}

// Create transform with given three columns
Transform3Dd::Transform3Dd(const Point3Dd& col0, const Point3Dd& col1,
			 const Point3Dd& col2) 
{
  setCol(0,col0);
  setCol(1,col1);
  setCol(2,col2);
}

// Create transform with given values (Row-major order!)
Transform3Dd::Transform3Dd(double r0c0, double r0c1, double r0c2,
			    double r1c0, double r1c1, double r1c2,
			    double r2c0, double r2c1, double r2c2)
{
  data[0][0] = r0c0;
  data[0][1] = r0c1;
  data[0][2] = r0c2;
  data[1][0] = r1c0;
  data[1][1] = r1c1;
  data[1][2] = r1c2;
  data[2][0] = r2c0;
  data[2][1] = r2c1;
  data[2][2] = r2c2;
};
  
// copy constructor
Transform3Dd::Transform3Dd(const Transform3Dd& other)
{
  *this = other;
}

// assignment operator
Transform3Dd& Transform3Dd::operator=(const Transform3Dd& other)
{
  if (this != &other) {
    for(int r=0;r<rowSize;r++)
      for (int c=0;c<colSize;c++)
	data[r][c] = other.data[r][c];
  }
  return *this;
}

// destructor
Transform3Dd::~Transform3Dd() {}

// access row r, column c
const double& Transform3Dd::entry(int row,int col) const
{
  if ((0 <= row) && (row < rowSize) && (0 <= col) && (col <= colSize))
    return data[row][col];
  else {
    std::cout << "Range error in function entry()" <<std::endl;
    exit(1);
  }
}

// access row r, column c
double& Transform3Dd::entry(int row,int col)
{
  if ((0 <= row) && (row < rowSize) && (0 <= col) && (col <= colSize))
    return data[row][col];
  else {
    std::cout << "Range error in function entry()" <<std::endl;
    exit(1);
  }
}

// copy and return row r or column c as a Point3Dd
Point3Dd Transform3Dd::getRow(int r) const
{
  Point3Dd temp;

  for(int c=0;c<colSize;c++)
    temp.data[c] = data[r][c];
  
  return temp;
}

Point3Dd Transform3Dd::getCol(int c) const
{
  Point3Dd temp;
 
  for(int r=0;r<rowSize;r++)
    temp.data[r] = data[r][c];
  
  return temp;
}

// replace row r or column c with Point3Dd
void Transform3Dd::setRow(int r,const Point3Dd& row)
{
  for(int c=0;c<colSize;c++)
    data[r][c] = row.data[c];
}

void Transform3Dd::setCol(int c,const Point3Dd& col)
{
  for(int r=0;r<rowSize;r++)
    data[r][c] = col.data[r];
}

// Matrix operations

// perform transpose on Transform
Transform3Dd& Transform3Dd::transposeSelf(void)
{
  for(int r=0;r<rowSize-1;r++)
    for(int c=r+1;c < colSize;c++) {
      double save = data[c][r];
      data[c][r] = data[r][c];
      data[r][c] = save;
    }
  return *this;
}

// compute transpose of Transform
Transform3Dd Transform3Dd::transpose(void) const
{
  Transform3Dd temp;

  for(int r=0;r<rowSize;r++)
    for(int c=0;c < colSize;c++)
      temp.data[c][r] = data[r][c];
  return temp;
}

// increment/decrement/multiply assignment operators
Transform3Dd& Transform3Dd::operator+=(const Transform3Dd& other)
{
  if((rowSize == other.rowSize) && (colSize == other.colSize)) {
    for(int r=0;r<rowSize;r++)
      for(int c=0;c<colSize;c++)
	data[r][c] += other.data[r][c];
    return *this;
  }
  else
    std::cout << "Matrix addition error: different shapes" <<std::endl;
}

Transform3Dd& Transform3Dd::operator-=(const Transform3Dd& other)
{
  if((rowSize == other.rowSize) && (colSize == other.colSize)) {
    for(int r=0;r<rowSize;r++)
      for(int c=0;c<colSize;c++)
	data[r][c] -= other.data[r][c];
    return *this;
  }
  else
    std::cout << "Matrix subtraction error: different shapes" <<std::endl;
}

// note: this is matrix multiplication--NOT co-ordinate-wise
Transform3Dd& Transform3Dd::operator*=(const Transform3Dd& other)
{
  Transform3Dd temp = *this;

  if(colSize == other.rowSize) {
    for(int r=0;r<rowSize;r++)
      for(int c=0;c<other.colSize;c++) {
	data[r][c] = 0; 
	for(int j=0;j<colSize;j++)
	  data[r][c] += temp.data[r][j]*other.data[j][c];
      }

    return *this;
  }
 else {
   std::cout << "Matrix product error: different shapes" <<std::endl;
   exit(1);
 }
}

// increment/decrement/multiply self by scalar
Transform3Dd& Transform3Dd::operator+=(const double scalar)
{
  for(int r=0;r<rowSize;r++)
    for(int c=0;c<colSize;c++)
      data[r][c] += scalar;
  
  return *this;
}

Transform3Dd& Transform3Dd::operator-=(const double scalar)
{
  for(int r=0;r<rowSize;r++)
    for(int c=0;c<colSize;c++)
      data[r][c] -= scalar;
  
  return *this;
}

Transform3Dd& Transform3Dd::operator*=(const double scalar)
{
  for(int r=0;r<rowSize;r++)
    for(int c=0;c<colSize;c++)
      data[r][c] *= scalar;
  
  return *this;
}

// overloaded binary operators

// matrix addition, subtraction and multiplication
Transform3Dd Transform3Dd::operator+(const Transform3Dd& m2) const
{
  Transform3Dd temp;
  temp = *this;
  temp += m2;
  return temp;
}

Transform3Dd Transform3Dd::operator-(const Transform3Dd& m2) const
{
  Transform3Dd temp;
  temp = *this;
  temp -= m2;
  return temp;
}

// note: this is matrix multiplication--NOT co-ordinate-wise
Transform3Dd Transform3Dd::operator*(const Transform3Dd& m2) const
{
  Transform3Dd temp;
  temp = *this;
  temp *= m2;
  return temp;
}

// operators for addition, subtraction, multiplication by a scalar
// always put matrix on LEFdouble side
Transform3Dd Transform3Dd::operator+(const double x) const
{
  Transform3Dd temp;

  temp = *this;
  temp += x;
  return temp;
}

Transform3Dd Transform3Dd::operator-(const double x) const
{
  Transform3Dd temp;

  temp = *this;
  temp -= x;
  return temp;
}

Transform3Dd Transform3Dd::operator*(const double x) const
{
  Transform3Dd temp;

  temp = *this;
  temp *= x;
  return temp;
}

// Apply matrix to point p
Point3Dd Transform3Dd::operator*(const Point3Dd& p) const
{
  Point3Dd result(0,0,0);

  for(int r=0;r<rowSize;r++){
    for(int c=0;c<colSize;c++)
      result.data[r] += data[r][c]*p.data[c];
  }
  return result;
}

// append transform to stream
std::ostream& Transform3Dd::out(std::ostream& o) const
{
  o << "(";
  for(int r=0;r<rowSize-1;r++) {
    o << "(";
    for(int c=0;c<colSize-1;c++) 
      o << entry(r,c) << ", ";
    o << entry(r,colSize-1) << "), ";
  }

// last row
  o << "(";
  for(int c=0;c<colSize-1;c++) 
    o << entry(rowSize-1,c) << ", ";
  o << entry(rowSize-1,colSize-1) << ")";
  
  o << ")" <<std::endl;

  return o;
}

// read Transform from stream
std::istream& Transform3Dd::in(std::istream& is)
{
  char c;
  Point3Dd input;

  is >> c;
  if (c != '(') {
    std::cout << "Bad format for Transform3Dd" <<std::endl;
    exit(1);
  }
  else {
    for(int r=0;r<rowSize-1;r++) {
      is >> input >> c;
      if (c != ',') {
	std::cout << "Bad format for Point3Dd" <<std::endl;
	exit(1);
      }
      for(int c=0;c<colSize;c++)
	data[r][c] = input.data[c];
    }
    is >> input >> c;
    if (c != ')') {
      std::cout << "Bad format for Point3Dd" <<std::endl;
      exit(1);
    }
    for(int c=0;c<colSize;c++)
      data[rowSize-1][c] = input.data[c];
  }

  return is;
}

// read transform from stream
std::istream& operator>>(std::istream& is, Transform3Dd& t)
{
  t.in(is);
}

// append transform to stream
std::ostream& operator<<(std::ostream& o, const Transform3Dd& t)
{
  return t.out(o);
}


