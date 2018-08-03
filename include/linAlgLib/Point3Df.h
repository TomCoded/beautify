// This may look like C code, but it is really -*- C++ -*-

// Point3Df.h

// (C) 2002 Bill Lenhart

#ifndef POINT3DF_H
#define POINT3DF_H

#include <iostream>

///////////////////
// class Point3Df //
///////////////////

// A 3-D point/vector of type float

class Transform3Df;

class Point3Df {

private:
  // number of co-ordinates
  static const int size = 3;

public:

  // Constructors
  // Initialize to (0,0,0)
  Point3Df();

  // Initialize to (xval, yval, zval)
  Point3Df(float xval, float yval, float zval);

  // copy constructor
  Point3Df(const Point3Df& other);

  // assignment operator
  Point3Df& operator=(const Point3Df& other);

  // destructor
  ~Point3Df();

  // where the coordinates are stored
  float data[size];

  // aliases to access x, y and z co-ordinates
  float& x; // data[0]
  float& y; // data[1]
  float& z; // data[2]

  // number of coordinates
  int length(void) const;

  // sum of coordinates
  float sum() const;

  // dot product
  float dot(const Point3Df& v2) const;
  
  // compute length (distance to origin)
  float norm() const;

  // normalize: divide self by self.norm()
  Point3Df& normalize();

  // compute cross product self x other
  Point3Df cross(const Point3Df& other) const;

  //co-ordinate-wise add,subtract,multiply to self
  Point3Df& operator+=(const Point3Df& other);
  Point3Df& operator-=(const Point3Df& other);
  Point3Df& operator*=(const Point3Df& other);
  
  // add, subtract, multiply, every element by scalar
  Point3Df& operator+=(const float scalar);
  Point3Df& operator-=(const float scalar);
  Point3Df& operator*=(const float scalar);
  
  //co-ordinate-wise addition, subtraction and multiplication
  Point3Df operator+(const Point3Df& v2) const;
  Point3Df operator-(const Point3Df& v2) const;
  Point3Df operator*(const Point3Df& v2) const;
  
  // operators for addition, subtraction, multiplication by scalar
  // always put point on the left
  Point3Df operator*(const float x) const;
  Point3Df operator+(const float x) const;
  Point3Df operator-(const float x) const;

  // Transformations of points: self = t*self
  Point3Df& applyToSelf(const Transform3Df& t);

  // Transformations of points: returns t*self
  Point3Df apply(const Transform3Df& t) const;

  // read point from stream
  istream& in(istream& is);
  
// append point to end of stream
  ostream& out(ostream&) const;
};

// read a point:  a binary operator
istream& operator>>(istream& is, Point3Df& p);

// print a point: a binary operator
ostream& operator<<(ostream& o, const Point3Df& p);

#endif POINT3DF_H




