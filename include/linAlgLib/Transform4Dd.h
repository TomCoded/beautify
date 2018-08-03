// This may look like C code, but it is really -*- C++ -*-

// Transform4Dd.h

// 4x4 matrix with simple operations

// (C) 2002 Bill Lenhart

#ifndef D4TRANSFORM_H
#define D4TRANSFORM_H

#include <iostream>


///////////////////////
// class Transform4Dd //
///////////////////////

// A 4x4 Matrix of type double
//
// Supports matrix/vector operations.
//

class Point4Dd;

class Transform4Dd {

private:
  // dimensions of transform
  static const int rowSize = 4;
  static const int colSize = 4;

public:
  // data for transform
  double data[rowSize][colSize];

  // Constructors
  // Default: create identity transform
  Transform4Dd();

  // Create transform with given three columns
  Transform4Dd(const Point4Dd& col0, const Point4Dd& col1,
	       const Point4Dd& col2, const Point4Dd& col3);

  // Create transform with given values (Row-major order!)
  Transform4Dd(double r0c0, double r0c1, double r0c2, double r0c3,
	      double r1c0, double r1c1, double r1c2, double r1c3,
	      double r2c0, double r2c1, double r2c2, double r2c3,
	      double r3c0, double r3c1, double r3c2, double r3c3);

  // copy constructor
  Transform4Dd(const Transform4Dd& other);

  // assignment operator
  Transform4Dd& operator=(const Transform4Dd& other);

  // destructor
  ~Transform4Dd();

  // access/update row r, column c
  double& entry(int r,int c);

  // access row r, column c
  const double& entry(int r,int c) const;

  // copy and return row r or column c as a Point4Dd
  Point4Dd getRow(int r) const;
  Point4Dd getCol(int c) const;

  // replace row r or column c with Point4Dd
  void setRow(int r, const Point4Dd& p);
  void setCol(int c, const Point4Dd& p);

  // perform transpose on Transform
  Transform4Dd& transposeSelf(void);

  // compute transpose of Transform
  Transform4Dd transpose(void) const;

  // increment/decrement/multiply assignment operators
  Transform4Dd& operator+=(const Transform4Dd& other);
  Transform4Dd& operator-=(const Transform4Dd& other);
  // note: this is matrix multiplication--NOT co-ordinate-wise
  Transform4Dd& operator*=(const Transform4Dd& other);

  // increment/decrement/multiply self by scalar
  Transform4Dd& operator+=(const double scalar);
  Transform4Dd& operator-=(const double scalar);
  Transform4Dd& operator*=(const double scalar);

  // overloaded binary operators

  // Transform4Dd addition, subtraction and multiplication
  Transform4Dd operator+(const Transform4Dd&) const;
  Transform4Dd operator-(const Transform4Dd&) const;
  // note: this is matrix multiplication--NOT co-ordinate-wise
  Transform4Dd operator*(const Transform4Dd&) const;
  
  // operators for addition, subtraction, multiplication by a scalar
  // always put matrix on LEFT side
  Transform4Dd operator+(const double scalar) const;
  Transform4Dd operator-(const double scalar) const;
  Transform4Dd operator*(const double scalar) const;

  // Apply matrix to Point4Dd p
  Point4Dd operator*(const Point4Dd&) const;

  // read Transform from stream
  std::istream& Transform4Dd::in(std::istream& is);

  // append transform to stream
  std::ostream& out(std::ostream&) const;

};
  
// read transform from stream
std::istream& operator>>(std::istream& is, Transform4Dd& t);

// append transform to stream
std::ostream& operator<<(std::ostream& o, const Transform4Dd& t);

#endif D4TRANSFORM_H
