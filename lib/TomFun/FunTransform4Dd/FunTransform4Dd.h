// FunTransform4Dd.h (C) 2002 Tom White -*- C++ -*-
//  contains class declaration for a 4-Dimensional transformation that
//  contains functions of a double value, (t)

#ifndef FUNTRANSFORM4DD_H_
#define FUNTRANSFORM4DD_H_

#include <Transform4Dd.h>

#ifndef POINT_TYPE_4D

#define POINT_TYPE_4D Point4Dd
#define PT4D_PX x
#define PT4D_PY y
#define PT4D_PZ z
#define PT4D_PW w

#endif

#include <iostream>

class POINT_TYPE_4D;
class FunNode;

class FunTransform4Dd {

private:
  // dimensions of transform
  static const int rowSize = 4;
  static const int colSize = 4;

public:
  // Constructors
  // Default: create identity transform
  FunTransform4Dd();

  bool destroyOnDeath;

  // Create transform with given three columns
  FunTransform4Dd(const POINT_TYPE_4D& col0, const POINT_TYPE_4D& col1,
	       const POINT_TYPE_4D& col2, const POINT_TYPE_4D& col3);

  // Create transform with given values (Row-major order!)
  FunTransform4Dd(double r0c0, double r0c1, double r0c2, double r0c3,
	      double r1c0, double r1c1, double r1c2, double r1c3,
	      double r2c0, double r2c1, double r2c2, double r2c3,
	      double r3c0, double r3c1, double r3c2, double r3c3);

  // Create transform with given values (Row-major order!)
  FunTransform4Dd(FunNode * r0c0, FunNode * r0c1, FunNode * r0c2,  FunNode * r0c3,
		  FunNode *r1c0, FunNode * r1c1, FunNode * r1c2, FunNode *r1c3,
		  FunNode *r2c0, FunNode * r2c1, FunNode * r2c2, FunNode *r2c3,
		  FunNode *r3c0, FunNode * r3c1, FunNode * r3c2, FunNode *r3c3);

  // copy constructor
  FunTransform4Dd(const FunTransform4Dd& other);

  // assignment operator
  FunTransform4Dd& operator=(const FunTransform4Dd& other);

  // destructor
  ~FunTransform4Dd();

  //get Transform4Dd for current time
  Transform4Dd getStaticTransform();
  
  // access/update row r, column c
  FunNode*& entry(int r,int c);

  // access row r, column c
  FunNode * entry(int r,int c) const;

  // copy and return row r or column c as a POINT_TYPE_4D
  //  POINT_TYPE_4D getRow(int r) const;
  //  POINT_TYPE_4D getCol(int c) const;

  // replace row r or column c with POINT_TYPE_4D
  void setRow(int r, const POINT_TYPE_4D& p);
  void setCol(int c, const POINT_TYPE_4D& p);

  // perform transpose on Transform
  FunTransform4Dd& transposeSelf(void);

  // compute transpose of Transform
  FunTransform4Dd transpose(void) const;

  // increment/decrement/multiply assignment operators
  FunTransform4Dd& operator+=(const FunTransform4Dd& other);
  FunTransform4Dd& operator-=(const FunTransform4Dd& other);
  // note: this is matrix multiplication--NOT co-ordinate-wise
  FunTransform4Dd& operator*=(const FunTransform4Dd& other);

  // increment/decrement/multiply self by scalar
  FunTransform4Dd& operator+=(const double scalar);
  FunTransform4Dd& operator-=(const double scalar);
  FunTransform4Dd& operator*=(const double scalar);

  // overloaded binary operators

  // FunTransform4Dd addition, subtraction and multiplication
  FunTransform4Dd operator+(const FunTransform4Dd&) const;
  FunTransform4Dd operator-(const FunTransform4Dd&) const;
  // note: this is matrix multiplication--NOT co-ordinate-wise
  FunTransform4Dd operator*(const FunTransform4Dd&) const;
  
  // operators for addition, subtraction, multiplication by a scalar
  // always put matrix on LEFT side
  FunTransform4Dd operator+(const double scalar) const;
  FunTransform4Dd operator-(const double scalar) const;
  FunTransform4Dd operator*(const double scalar) const;

  // Apply matrix to POINT_TYPE_4D p
  POINT_TYPE_4D operator*(const POINT_TYPE_4D&) const;

  double t; //the current time, for evaluating the functions

  // data for transform
  FunNode * data[rowSize][colSize];

  // read Transform from stream
  istream& FunTransform4Dd::in(istream& is);

  // append transform to stream
  ostream& out(ostream&) const;

};
  
// read transform from stream
istream& operator>>(istream& is, FunTransform4Dd& t);

// append transform to stream
ostream& operator<<(ostream& o, const FunTransform4Dd& t);

#endif D4TRANSFORM_H
