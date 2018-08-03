// This may look like C code, but it is really -*- C++ -*-

// Transform3Dd.h

// 3x3 matrix with simple operations

// (C) 2002 Bill Lenhart

#ifndef TRANSFORM3DD_H
#define TRANSFORM3DD_H

#include <iostream>


///////////////////////
// class Transform3Dd //
///////////////////////

// A 3x3 Matrix of type double
//
// Supports matrix/vector operations.
//

class Point3Dd;

class Transform3Dd {

private:
  // dimensions of transform
  static const int rowSize = 3;
  static const int colSize = 3;

public:
  // data for transform
  double data[rowSize][colSize];

  // Constructors
  // Default: create identity transform
  Transform3Dd();

  // Create transform with given three columns
  Transform3Dd(const Point3Dd& col0, const Point3Dd& col1, const Point3Dd& col2);

  // Create transform with given values (Row-major order!)
  Transform3Dd(double r0c0, double r0c1, double r0c2,
	      double r1c0, double r1c1, double r1c2,
	      double r2c0, double r2c1, double r2c2);

  // copy constructor
  Transform3Dd(const Transform3Dd& other);

  // assignment operator
  Transform3Dd& operator=(const Transform3Dd& other);

  // destructor
  ~Transform3Dd();

  // access/update row r, column c
  double& entry(int r,int c);

  // access row r, column c
  const double& entry(int r,int c) const;

  // copy and return row r or column c as a Point3Dd
  Point3Dd getRow(int r) const;
  Point3Dd getCol(int c) const;

  // replace row r or column c with Point3Dd
  void setRow(int r, const Point3Dd& row);
  void setCol(int c, const Point3Dd& col);

  // perform transpose on Transform
  Transform3Dd& transposeSelf(void);

  // compute transpose of Transform
  Transform3Dd transpose(void) const;

  // increment/decrement/multiply assignment operators
  Transform3Dd& operator+=(const Transform3Dd& other);
  Transform3Dd& operator-=(const Transform3Dd& other);
  // note: this is matrix multiplication--NOT co-ordinate-wise
  Transform3Dd& operator*=(const Transform3Dd& other);

  // increment/decrement/multiply self by scalar
  Transform3Dd& operator+=(const double scalar);
  Transform3Dd& operator-=(const double scalar);
  Transform3Dd& operator*=(const double scalar);

  // overloaded binary operators

  // Transform3Dd addition, subtraction and multiplication
  Transform3Dd operator+(const Transform3Dd&) const;
  Transform3Dd operator-(const Transform3Dd&) const;
  // note: this is matrix multiplication--NOT co-ordinate-wise
  Transform3Dd operator*(const Transform3Dd&) const;
  
  // operators for addition, subtraction, multiplication by a scalar
  // always put matrix on LEFT side
  Transform3Dd operator+(const double scalar) const;
  Transform3Dd operator-(const double scalar) const;
  Transform3Dd operator*(const double scalar) const;

  // Apply matrix to Point3Dd p
  Point3Dd operator*(const Point3Dd&) const;

  // read transform from stream
  std::istream& in(std::istream& is);

  // append transform to stream
  std::ostream& out(std::ostream&) const;

};
  
// read transform from stream
istream& operator>>(std::istream& is, Transform3Dd& t);

// append transform to stream
ostream& operator<<(std::ostream& o, const Transform3Dd& t);

#endif TRANSFORM3DD_H
