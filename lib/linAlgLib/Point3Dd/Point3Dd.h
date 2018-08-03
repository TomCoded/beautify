// This may look like C code, but it is really -*- C++ -*-

// Point3Dd.h

// (C) 2002 Bill Lenhart

#ifndef POINT3DD_H
#define POINT3DD_H

#include <iostream>


///////////////////
// class Point3Dd //
///////////////////

// A 3-D point/vector of type double

class Transform3Dd;

class Point3Dd {

private:
  // number of co-ordinates
  static const int size = 3;

public:

  // Constructors
  // Initialize to (0,0,0)
  Point3Dd();

  // Initialize to (xval, yval, zval)
  Point3Dd(double xval, double yval, double zval);

  // copy constructor
  Point3Dd(const Point3Dd& other);

  // assignment operator
  Point3Dd& operator=(const Point3Dd& other);

  // destructor
  ~Point3Dd();

  // where the coordinates are stored
  double data[size];

  // aliases to access x, y and z co-ordinates
  double& x; // data[0]
  double& y; // data[1]
  double& z; // data[2]

  // number of coordinates
  int length(void) const;

  // sum of coordinates
  double sum() const;

  // dot product
  double dot(const Point3Dd& v2) const;
  
  // compute length (distance to origin)
  double norm() const;

  // normalize: divide self by self.norm()
  Point3Dd& normalize();

  // compute cross product self x other
  Point3Dd cross(const Point3Dd& other) const;

  //co-ordinate-wise add,subtract,multiply to self
  Point3Dd& operator+=(const Point3Dd& other);
  Point3Dd& operator-=(const Point3Dd& other);
  Point3Dd& operator*=(const Point3Dd& other);
  
  // add, subtract, multiply, every element by scalar
  Point3Dd& operator+=(const double scalar);
  Point3Dd& operator-=(const double scalar);
  Point3Dd& operator*=(const double scalar);
  
  //co-ordinate-wise addition, subtraction and multiplication
  Point3Dd operator+(const Point3Dd& v2) const;
  Point3Dd operator-(const Point3Dd& v2) const;
  Point3Dd operator*(const Point3Dd& v2) const;
  
  // operators for addition, subtraction, multiplication by scalar
  // always put point on the left
  Point3Dd operator*(const double x) const;
  Point3Dd operator+(const double x) const;
  Point3Dd operator-(const double x) const;

  // Transformations of points: self = t*self
  Point3Dd& applyToSelf(const Transform3Dd& t);

  // Transformations of points: returns t*self
  Point3Dd apply(const Transform3Dd& t) const;

  // read point from stream
  std::istream& in(std::istream& is);
  
// append point to end of stream
  std::ostream& out(std::ostream&) const;
};

// read a point:  a binary operator
istream& operator>>(std::istream& is, Point3Dd& p);

// print a point: a binary operator
ostream& operator<<(std::ostream& o, const Point3Dd& p);

#endif POINT3DD_H




