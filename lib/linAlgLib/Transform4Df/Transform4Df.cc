// Transform4Df.C

// 4x4 matrix with simple operations

// (C) 2002 Bill Lenhart

#include <stdlib.h>
#include "Point4Df/Point4Df.h"
#include "Transform4Df/Transform4Df.h"


// Constructors
// Default: create identity transform
Transform4Df::Transform4Df() 
{
  for (int r=0;r<rowSize;r++)
    for (int c=0;c<colSize;c++)
      if(r==c)
	data[r][c] = 1;
      else
	data[r][c] = 0;
}

// Create transform with given three columns
Transform4Df::Transform4Df(const Point4Df& col0, const Point4Df& col1,
			   const Point4Df& col2, const Point4Df& col3)
{
  setCol(0,col0);
  setCol(1,col1);
  setCol(2,col2);
  setCol(3,col3);
}

// Create transform with given values (Row-major order!)
Transform4Df::Transform4Df(float r0c0, float r0c1, float r0c2, float r0c3,
			 float r1c0, float r1c1, float r1c2, float r1c3,
			 float r2c0, float r2c1, float r2c2, float r2c3,
			 float r3c0, float r3c1, float r3c2, float r3c3)
{
  data[0][0] = r0c0;
  data[0][1] = r0c1;
  data[0][2] = r0c2;
  data[0][3] = r0c3;
  data[1][0] = r1c0;
  data[1][1] = r1c1;
  data[1][2] = r1c2;
  data[1][3] = r1c3;
  data[2][0] = r2c0;
  data[2][1] = r2c1;
  data[2][2] = r2c2;
  data[2][3] = r2c3;
  data[3][0] = r3c0;
  data[3][1] = r3c1;
  data[3][2] = r3c2;
  data[3][3] = r3c3;
};
  
// copy constructor
Transform4Df::Transform4Df(const Transform4Df& other)
{
  *this = other;
}

// assignment operator
Transform4Df& Transform4Df::operator=(const Transform4Df& other)
{
  if (this != &other) {
    for(int r=0;r<rowSize;r++)
      for (int c=0;c<colSize;c++)
	data[r][c] = other.data[r][c];
  }
  return *this;
}

Transform4Df::~Transform4Df() {}

// access row r, column c
const float& Transform4Df::entry(int row,int col) const
{
  if ((0 <= row) && (row < rowSize) && (0 <= col) && (col <= colSize))
    return data[row][col];
  else {
    cout << "Range error in function entry()" << endl;
    exit(1);
  }
}

// access row r, column c
float& Transform4Df::entry(int row,int col)
{
  if ((0 <= row) && (row < rowSize) && (0 <= col) && (col <= colSize))
    return data[row][col];
  else {
    cout << "Range error in function entry()" << endl;
    exit(1);
  }
}

// copy and return row r or column c as a Point4Df
Point4Df Transform4Df::getRow(int r) const
{
  Point4Df temp;
 
  for(int c=0;c<colSize;c++)
    temp.data[c] = data[r][c];
  
  return temp;
}

Point4Df Transform4Df::getCol(int c) const
{
  Point4Df temp;

  for(int r=0;r<rowSize;r++)
    temp.data[r] = data[r][c];
  
  return temp;
}

// replace row r or column c with Point4Df
void Transform4Df::setRow(int r,const Point4Df& row)
{
  for(int c=0;c<colSize;c++)
    data[r][c] = row.data[c];
}

void Transform4Df::setCol(int c,const Point4Df& col)
{
  for(int r=0;r<rowSize;r++)
    data[r][c] = col.data[r];
}

// Matrix operations

// perform transpose on Transform
Transform4Df& Transform4Df::transposeSelf(void)
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
Transform4Df Transform4Df::transpose(void) const
{
  Transform4Df temp;

  for(int r=0;r<rowSize;r++)
    for(int c=0;c < colSize;c++)
      temp.data[c][r] = data[r][c];
  return temp;
}

// increment/decrement/multiply assignment operators
Transform4Df& Transform4Df::operator+=(const Transform4Df& other)
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

Transform4Df& Transform4Df::operator-=(const Transform4Df& other)
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
Transform4Df& Transform4Df::operator*=(const Transform4Df& other)
{
  Transform4Df temp = *this;

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
Transform4Df& Transform4Df::operator+=(const float scalar)
{
  for(int r=0;r<rowSize;r++)
    for(int c=0;c<colSize;c++)
      data[r][c] += scalar;
  
  return *this;
}

Transform4Df& Transform4Df::operator-=(const float scalar)
{
  for(int r=0;r<rowSize;r++)
    for(int c=0;c<colSize;c++)
      data[r][c] -= scalar;
  
  return *this;
}

Transform4Df& Transform4Df::operator*=(const float scalar)
{
  for(int r=0;r<rowSize;r++)
    for(int c=0;c<colSize;c++)
      data[r][c] *= scalar;
  
  return *this;
}

// overloaded binary operators

// matrix addition, subtraction and multiplication
Transform4Df Transform4Df::operator+(const Transform4Df& m2) const
{
  Transform4Df temp;
  temp = *this;
  temp += m2;
  return temp;
}

Transform4Df Transform4Df::operator-(const Transform4Df& m2) const
{
  Transform4Df temp;
  temp = *this;
  temp -= m2;
  return temp;
}

// note: this is matrix multiplication--NOT co-ordinate-wise
Transform4Df Transform4Df::operator*(const Transform4Df& m2) const
{
  Transform4Df temp;
  temp = *this;
  temp *= m2;
  return temp;
}

// operators for addition, subtraction, multiplication by a scalar
// always put matrix on LEFfloat side
Transform4Df Transform4Df::operator+(const float x) const
{
  Transform4Df temp;

  temp = *this;
  temp += x;
  return temp;
}

Transform4Df Transform4Df::operator-(const float x) const
{
  Transform4Df temp;

  temp = *this;
  temp -= x;
  return temp;
}

Transform4Df Transform4Df::operator*(const float x) const
{
  Transform4Df temp;

  temp = *this;
  temp *= x;
  return temp;
}

// Apply matrix to Point4Df p
Point4Df Transform4Df::operator*(const Point4Df& v) const
{
  Point4Df result(0,0,0,0);

  for(int r=0;r<rowSize;r++){
    for(int c=0;c<colSize;c++)
      result.data[r] += data[r][c]*v.data[c];
  }
  return result;
}

// append transform to stream
ostream& Transform4Df::out(std::ostream& o) const
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
istream& Transform4Df::in(std::istream& is)
{
  char c;
  Point4Df input;

  is >> c;
  if (c != '(') {
    cout << "Bad format for Transform4Df" << endl;
    exit(1);
  }
  else {
    for(int r=0;r<rowSize-1;r++) {
      is >> input >> c;
      if (c != ',') {
	cout << "Bad format for Point4Df" << endl;
	exit(1);
      }
      for(int c=0;c<colSize;c++)
	data[r][c] = input.data[c];
    }
    is >> input >> c;
    if (c != ')') {
      cout << "Bad format for Point4Df" << endl;
      exit(1);
    }
    for(int c=0;c<colSize;c++)
      data[rowSize-1][c] = input.data[c];
  }

  return is;
}

// read transform from stream
istream& operator>>(std::istream& is, Transform4Df& t)
{
  t.in(is);
}

// append transform to stream
ostream& operator<<(std::ostream& o, const Transform4Df& t)
{
  return t.out(o);
}


