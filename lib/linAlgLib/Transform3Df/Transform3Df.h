// This may look like C code, but it is really -*- C++ -*-

// Transform3Df.h

// 3x3 matrix with simple operations

// (C) 2002 Bill Lenhart

#ifndef TRANSFORM3DF_H
#define TRANSFORM3DF_H

#include <iostream>
using namespace std;

///////////////////////
// class Transform3Df //
///////////////////////

// A 3x3 Matrix of type float
//
// Supports matrix/vector operations.
//

class Point3Df;

class Transform3Df {

private:
  // dimensions of transform
  static const int rowSize = 3;
  static const int colSize = 3;

public:
  // data for transform
  float data[rowSize][colSize];

  // Constructors
  // Default: create identity transform
  Transform3Df();

  // Create transform with given three columns
  Transform3Df(const Point3Df& col0, const Point3Df& col1, const Point3Df& col2);

  // Create transform with given values (Row-major order!)
  Transform3Df(float r0c0, float r0c1, float r0c2,
	      float r1c0, float r1c1, float r1c2,
	      float r2c0, float r2c1, float r2c2);

  // copy constructor
  Transform3Df(const Transform3Df& other);

  // assignment operator
  Transform3Df& operator=(const Transform3Df& other);

  // destructor
  ~Transform3Df();

  // access/update row r, column c
  float& entry(int r,int c);

  // access row r, column c
  const float& entry(int r,int c) const;

  // copy and return row r or column c as a Point3Df
  Point3Df getRow(int r) const;
  Point3Df getCol(int c) const;

  // replace row r or column c with Point3Df
  void setRow(int r, const Point3Df& row);
  void setCol(int c, const Point3Df& col);

  // perform transpose on Transform
  Transform3Df& transposeSelf(void);

  // compute transpose of Transform
  Transform3Df transpose(void) const;

  // increment/decrement/multiply assignment operators
  Transform3Df& operator+=(const Transform3Df& other);
  Transform3Df& operator-=(const Transform3Df& other);
  // note: this is matrix multiplication--NOT co-ordinate-wise
  Transform3Df& operator*=(const Transform3Df& other);

  // increment/decrement/multiply self by scalar
  Transform3Df& operator+=(const float scalar);
  Transform3Df& operator-=(const float scalar);
  Transform3Df& operator*=(const float scalar);

  // overloaded binary operators

  // Transform3Df addition, subtraction and multiplication
  Transform3Df operator+(const Transform3Df&) const;
  Transform3Df operator-(const Transform3Df&) const;
  // note: this is matrix multiplication--NOT co-ordinate-wise
  Transform3Df operator*(const Transform3Df&) const;
  
  // operators for addition, subtraction, multiplication by a scalar
  // always put matrix on LEFT side
  Transform3Df operator+(const float scalar) const;
  Transform3Df operator-(const float scalar) const;
  Transform3Df operator*(const float scalar) const;

  // Apply matrix to Point3Df p
  Point3Df operator*(const Point3Df&) const;

  // read transform from stream
  istream& in(istream& is);

  // append transform to stream
  ostream& out(ostream&) const;

};
  
// read transform from stream
istream& operator>>(istream& is, Transform3Df& t);

// append transform to stream
ostream& operator<<(ostream& o, const Transform3Df& t);

#endif TRANSFORM3DF_H
