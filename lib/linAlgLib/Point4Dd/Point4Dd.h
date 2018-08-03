// This may look like C code, but it is really -*- C++ -*-

// Point4Dd.h

// (C) 2002 Bill Lenhart

#ifndef POINT4DD_H
#define POINT4DD_H

#include <iostream>
using namespace std;

///////////////////
// class Point4Dd //
///////////////////

// A 4-D point/vector of type double

class Point3Dd;
class Transform4Dd;

class Point4Dd {

private:
  // number of co-ordinates
  static const int size = 4;

public:

  // Constructors
  // Initialize to (0,0,0,0)
  Point4Dd();

  // Initialize to (xval, yval, zval, wval)
  Point4Dd(double xval, double yval, double zval, double wval);

  // Initialize to (Point3Dd p, wval)
  Point4Dd(const Point3Dd& p, double wval);

  // copy constructor
  Point4Dd(const Point4Dd& other);

  // assignment operator
  Point4Dd& operator=(const Point4Dd& other);

  // destructor
  ~Point4Dd();

  // where the coordinates are stored
  double data[size];

  // aliases to access x, y, z and w co-ordinates
  double& x; // data[0]
  double& y; // data[1]
  double& z; // data[2]
  double& w; // data[3]

  // homogenize a Point3Dd p with 4th coord wval
  static Point4Dd homogenize(const Point3Dd& p, double wval);

  // dehomogenize a Point4Dd: if w != 0, return (x/w, y/w, z/w)
  Point3Dd dehomogenize() const;

  // number of coordinates
  int length(void) const;

  // sum of coordinates
  double sum() const;

  // dot product
  double dot(const Point4Dd& v2) const;
  
  // compute length of vector (distance to origin)
  double norm() const;

  // normalize vector: divide self by self.norm()
  Point4Dd& normalize();

  //co-ordinate-wise add,subtract,multiply to self
  Point4Dd& operator+=(const Point4Dd& other);
  Point4Dd& operator-=(const Point4Dd& other);
  Point4Dd& operator*=(const Point4Dd& other);
  
  // add, subtract, multiply, every element by scalar
  Point4Dd& operator+=(const double scalar);
  Point4Dd& operator-=(const double scalar);
  Point4Dd& operator*=(const double scalar);
  
  //co-ordinate-wise addition, subtraction and multiplication
  Point4Dd operator+(const Point4Dd& v2) const;
  Point4Dd operator-(const Point4Dd& v2) const;
  Point4Dd operator*(const Point4Dd& v2) const;
  
  // operators for addition, subtraction, multiplication by scalar
  // always put point on the left
  Point4Dd operator*(const double x) const;
  Point4Dd operator+(const double x) const;
  Point4Dd operator-(const double x) const;

  // Transformations of points: self = t*self
  Point4Dd& applyToSelf(const Transform4Dd& t);

  // Transformations of points: returns t*self
  Point4Dd apply(const Transform4Dd& t) const;

  // read point from stream
  istream& in(istream& is);
  
// append point to end of stream
  ostream& out(ostream&) const;
};

// read a point:  a binary operator
istream& operator>>(istream& is, Point4Dd& p);

// print a point: a binary operator
ostream& operator<<(ostream& o, const Point4Dd& p);

#endif POINT4DD_H




