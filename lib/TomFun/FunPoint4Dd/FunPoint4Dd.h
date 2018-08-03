// FunPoint4Dd.h (C) 2002 Tom White -*- C++ -*-
//  contains class declaration for a 4-Dimensional transformation that
//  contains functions of a double value, (t)

#ifndef FUNPOINT4DD_H_
#define FUNPOINT4DD_H_

#ifndef POINT_TYPE_4D

#define POINT_TYPE_4D Point4Dd

#include <Point4Dd.h>

#define PT4D_PX x
#define PT4D_PY y
#define PT4D_PZ z
#define PT4D_PW w

#endif

#include <iostream>

class POINT_TYPE_4D;
class FunNode;

class FunPoint4Dd {

private:
  // dimensions of transform
  static const int size = 4;

public:
  // Constructors
  // Default: 0,0,0,1
  FunPoint4Dd();

  bool destroyOnDeath;

  // Create transform with given elt
  FunPoint4Dd(const POINT_TYPE_4D& col0);

  // Create transform with given values
  FunPoint4Dd(double c0, double c1, double c2, double c3);

  // Create transform with given values
  FunPoint4Dd(FunNode * c0, FunNode * c1, FunNode * c2,  FunNode * c3);

  // copy constructor
  FunPoint4Dd(const FunPoint4Dd& other);

  // assignment operator
  FunPoint4Dd& operator=(const FunPoint4Dd& other);

  // destructor
  ~FunPoint4Dd();

  //get 4Dd for current time
  POINT_TYPE_4D getStaticPoint();
  
  //eval get 4Dd for passed time
  POINT_TYPE_4D eval(double t);

  // increment/decrement assignment operators
  FunPoint4Dd& operator+=(const FunPoint4Dd& other);
  FunPoint4Dd& operator-=(const FunPoint4Dd& other);

  // increment/decrement/multiply self by scalar
  FunPoint4Dd& operator+=(const double scalar);
  FunPoint4Dd& operator-=(const double scalar);
  FunPoint4Dd& operator*=(const double scalar);

  // overloaded binary operators

  // FunPoint4Dd addition, subtraction and multiplication
  FunPoint4Dd operator+(const FunPoint4Dd&) const;
  FunPoint4Dd operator-(const FunPoint4Dd&) const;
  
  // operators for addition, subtraction, multiplication by a scalar
  // always put matrix on LEFT side
  FunPoint4Dd operator+(const double scalar) const;
  FunPoint4Dd operator-(const double scalar) const;
  FunPoint4Dd operator*(const double scalar) const;

  double t; //the current time, for evaluating the functions

  // data for point
  FunNode * data[size];

  // read Transform from stream
  std::istream& in(std::istream& is);

  // append transform to stream
  std::ostream& out(std::ostream&) const;

};
  
// read transform from stream
istream& operator>>(std::istream& is, FunPoint4Dd& t);

// append transform to stream
ostream& operator<<(std::ostream& o, const FunPoint4Dd& t);

#endif D4TRANSFORM_H
