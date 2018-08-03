// This may look like C code, but it is really -*- C++ -*-

// Transform4Df.h

// 4x4 matrix with simple operations

// (C) 2002 Bill Lenhart

#ifndef TRANSFORM4DF_H
#define TRANSFORM4DF_H

#include <iostream>


///////////////////////
// class Transform4Df //
///////////////////////

// A 4x4 Matrix of type float
//
// Supports matrix/vector operations.
//

class Point4Df;

class Transform4Df {

private:
  // dimensions of transform
  static const int rowSize = 4;
  static const int colSize = 4;

public:
  // data for transform
  float data[rowSize][colSize];

  // Constructors
  // Default: create identity transform
  Transform4Df();

  // Create transform with given three columns
  Transform4Df(const Point4Df& col0, const Point4Df& col1,
	       const Point4Df& col2, const Point4Df& col3);

  // Create transform with given values (Row-major order!)
  Transform4Df(float r0c0, float r0c1, float r0c2, float r0c3,
	      float r1c0, float r1c1, float r1c2, float r1c3,
	      float r2c0, float r2c1, float r2c2, float r2c3,
	      float r3c0, float r3c1, float r3c2, float r3c3);

  // copy constructor
  Transform4Df(const Transform4Df& other);

  // assignment operator
  Transform4Df& operator=(const Transform4Df& other);

  // destructor
  ~Transform4Df();

  // access/update row r, column c
  float& entry(int r,int c);

  // access row r, column c
  const float& entry(int r,int c) const;

  // copy and return row r or column c as a Point4Df
  Point4Df getRow(int r) const;
  Point4Df getCol(int c) const;

  // replace row r or column c with Point4Df
  void setRow(int r, const Point4Df& p);
  void setCol(int c, const Point4Df& p);

  // perform transpose on Transform
  Transform4Df& transposeSelf(void);

  // compute transpose of Transform
  Transform4Df transpose(void) const;

  // increment/decrement/multiply assignment operators
  Transform4Df& operator+=(const Transform4Df& other);
  Transform4Df& operator-=(const Transform4Df& other);
  // note: this is matrix multiplication--NOT co-ordinate-wise
  Transform4Df& operator*=(const Transform4Df& other);

  // increment/decrement/multiply self by scalar
  Transform4Df& operator+=(const float scalar);
  Transform4Df& operator-=(const float scalar);
  Transform4Df& operator*=(const float scalar);

  // overloaded binary operators

  // Transform4Df addition, subtraction and multiplication
  Transform4Df operator+(const Transform4Df&) const;
  Transform4Df operator-(const Transform4Df&) const;
  // note: this is matrix multiplication--NOT co-ordinate-wise
  Transform4Df operator*(const Transform4Df&) const;
  
  // operators for addition, subtraction, multiplication by a scalar
  // always put matrix on LEFT side
  Transform4Df operator+(const float scalar) const;
  Transform4Df operator-(const float scalar) const;
  Transform4Df operator*(const float scalar) const;

  // Apply matrix to Point4Df p
  Point4Df operator*(const Point4Df&) const;

  // read Transform from stream
  std::istream& in(std::istream& is);

  // append transform to stream
  std::ostream& out(std::ostream&) const;

};
  
// read transform from stream
istream& operator>>(std::istream& is, Transform4Df& t);

// append transform to stream
ostream& operator<<(std::ostream& o, const Transform4Df& t);

#endif TRANSFORM4DF_H
