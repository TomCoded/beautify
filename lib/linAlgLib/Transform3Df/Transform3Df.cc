// Transform3Df.C

// 3x3 matrix with simple operations

// (C) 2002 Bill Lenhart

#include <stdlib.h>
#include "Point3Df/Point3Df.h"
#include "Transform3Df/Transform3Df.h"

// Constructors
// Default: create identity transform
Transform3Df::Transform3Df() 
{
  for (int r=0;r<rowSize;r++)
    for (int c=0;c<colSize;c++)
      if(r==c)
	data[r][c] = 1;
      else
	data[r][c] = 0;
}

// Create transform with given three columns
Transform3Df::Transform3Df(const Point3Df& col0, const Point3Df& col1,
			 const Point3Df& col2) 
{
  setCol(0,col0);
  setCol(1,col1);
  setCol(2,col2);
}

// Create transform with given values (Row-major order!)
Transform3Df::Transform3Df(float r0c0, float r0c1, float r0c2,
			    float r1c0, float r1c1, float r1c2,
			    float r2c0, float r2c1, float r2c2)
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
Transform3Df::Transform3Df(const Transform3Df& other)
{
  *this = other;
}

// assignment operator
Transform3Df& Transform3Df::operator=(const Transform3Df& other)
{
  if (this != &other) {
    for(int r=0;r<rowSize;r++)
      for (int c=0;c<colSize;c++)
	data[r][c] = other.data[r][c];
  }
  return *this;
}

// destructor
Transform3Df::~Transform3Df() {}

// access row r, column c
const float& Transform3Df::entry(int row,int col) const
{
  if ((0 <= row) && (row < rowSize) && (0 <= col) && (col <= colSize))
    return data[row][col];
  else {
    cout << "Range error in function entry()" << endl;
    exit(1);
  }
}

// access row r, column c
float& Transform3Df::entry(int row,int col)
{
  if ((0 <= row) && (row < rowSize) && (0 <= col) && (col <= colSize))
    return data[row][col];
  else {
    cout << "Range error in function entry()" << endl;
    exit(1);
  }
}

// copy and return row r or column c as a Point3Df
Point3Df Transform3Df::getRow(int r) const
{
  Point3Df temp;

  for(int c=0;c<colSize;c++)
    temp.data[c] = data[r][c];
  
  return temp;
}

Point3Df Transform3Df::getCol(int c) const
{
  Point3Df temp;
 
  for(int r=0;r<rowSize;r++)
    temp.data[r] = data[r][c];
  
  return temp;
}

// replace row r or column c with Point3Df
void Transform3Df::setRow(int r,const Point3Df& row)
{
  for(int c=0;c<colSize;c++)
    data[r][c] = row.data[c];
}

void Transform3Df::setCol(int c,const Point3Df& col)
{
  for(int r=0;r<rowSize;r++)
    data[r][c] = col.data[r];
}

// Matrix operations

// perform transpose on Transform
Transform3Df& Transform3Df::transposeSelf(void)
{
  for(int r=0;r<rowSize-1;r++)
    for(int c=r+1;c < colSize;c++) {
      float save = data[c][r];
      data[c][r] = data[r][c];
      data[r][c] = save;
    }
  return *this;
}

// compute transpose of Transform
Transform3Df Transform3Df::transpose(void) const
{
  Transform3Df temp;

  for(int r=0;r<rowSize;r++)
    for(int c=0;c < colSize;c++)
      temp.data[c][r] = data[r][c];
  return temp;
}

// increment/decrement/multiply assignment operators
Transform3Df& Transform3Df::operator+=(const Transform3Df& other)
{
  if((rowSize == other.rowSize) && (colSize == other.colSize)) {
    for(int r=0;r<rowSize;r++)
      for(int c=0;c<colSize;c++)
	data[r][c] += other.data[r][c];
    return *this;
  }
  else
    cout << "Matrix addition error: different shapes" << endl;
}

Transform3Df& Transform3Df::operator-=(const Transform3Df& other)
{
  if((rowSize == other.rowSize) && (colSize == other.colSize)) {
    for(int r=0;r<rowSize;r++)
      for(int c=0;c<colSize;c++)
	data[r][c] -= other.data[r][c];
    return *this;
  }
  else
    cout << "Matrix subtraction error: different shapes" << endl;
}

// note: this is matrix multiplication--NOT co-ordinate-wise
Transform3Df& Transform3Df::operator*=(const Transform3Df& other)
{
  Transform3Df temp = *this;

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
   cout << "Matrix product error: different shapes" << endl;
   exit(1);
 }
}

// increment/decrement/multiply self by scalar
Transform3Df& Transform3Df::operator+=(const float scalar)
{
  for(int r=0;r<rowSize;r++)
    for(int c=0;c<colSize;c++)
      data[r][c] += scalar;
  
  return *this;
}

Transform3Df& Transform3Df::operator-=(const float scalar)
{
  for(int r=0;r<rowSize;r++)
    for(int c=0;c<colSize;c++)
      data[r][c] -= scalar;
  
  return *this;
}

Transform3Df& Transform3Df::operator*=(const float scalar)
{
  for(int r=0;r<rowSize;r++)
    for(int c=0;c<colSize;c++)
      data[r][c] *= scalar;
  
  return *this;
}

// overloaded binary operators

// matrix addition, subtraction and multiplication
Transform3Df Transform3Df::operator+(const Transform3Df& m2) const
{
  Transform3Df temp;
  temp = *this;
  temp += m2;
  return temp;
}

Transform3Df Transform3Df::operator-(const Transform3Df& m2) const
{
  Transform3Df temp;
  temp = *this;
  temp -= m2;
  return temp;
}

// note: this is matrix multiplication--NOT co-ordinate-wise
Transform3Df Transform3Df::operator*(const Transform3Df& m2) const
{
  Transform3Df temp;
  temp = *this;
  temp *= m2;
  return temp;
}

// operators for addition, subtraction, multiplication by a scalar
// always put matrix on LEFfloat side
Transform3Df Transform3Df::operator+(const float x) const
{
  Transform3Df temp;

  temp = *this;
  temp += x;
  return temp;
}

Transform3Df Transform3Df::operator-(const float x) const
{
  Transform3Df temp;

  temp = *this;
  temp -= x;
  return temp;
}

Transform3Df Transform3Df::operator*(const float x) const
{
  Transform3Df temp;

  temp = *this;
  temp *= x;
  return temp;
}

// Apply matrix to point p
Point3Df Transform3Df::operator*(const Point3Df& p) const
{
  Point3Df result(0,0,0);

  for(int r=0;r<rowSize;r++){
    for(int c=0;c<colSize;c++)
      result.data[r] += data[r][c]*p.data[c];
  }
  return result;
}

// append transform to stream
ostream& Transform3Df::out(std::ostream& o) const
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
  
  o << ")" << endl;

  return o;
}

// read Transform from stream
istream& Transform3Df::in(std::istream& is)
{
  char c;
  Point3Df input;

  is >> c;
  if (c != '(') {
    cout << "Bad format for Transform3Df" << endl;
    exit(1);
  }
  else {
    for(int r=0;r<rowSize-1;r++) {
      is >> input >> c;
      if (c != ',') {
	cout << "Bad format for Point3Df" << endl;
	exit(1);
      }
      for(int c=0;c<colSize;c++)
	data[r][c] = input.data[c];
    }
    is >> input >> c;
    if (c != ')') {
      cout << "Bad format for Point3Df" << endl;
      exit(1);
    }
    for(int c=0;c<colSize;c++)
      data[rowSize-1][c] = input.data[c];
  }

  return is;
}

// read transform from stream
istream& operator>>(std::istream& is, Transform3Df& t)
{
  t.in(is);
}

// append transform to stream
ostream& operator<<(std::ostream& o, const Transform3Df& t)
{
  return t.out(o);
}


