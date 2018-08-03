// This may look like C code, but it is really -*- C++ -*-

// Point4Df.h

// (C) 2002 Bill Lenhart

#ifndef POINT4DF_H
#define POINT4DF_H

#include <iostream>


///////////////////
// class Point4Df //
///////////////////

// A 4-D point/vector of type float

class Point3Df;
class Transform4Df;

class Point4Df {

private:
  // number of co-ordinates
  static const int size = 4;

public:

  // Constructors
  // Initialize to (0,0,0,0)
  Point4Df();

  // Initialize to (xval, yval, zval)
  Point4Df(float xval, float yval, float zval, float wval);

  // Initialize to (Point3Df p, wval)
  Point4Df(const Point3Df& p, float wval);

  // copy constructor
  Point4Df(const Point4Df& other);

  // assignment operator
  Point4Df& operator=(const Point4Df& other);

  // destructor
  ~Point4Df();

  // where the coordinates are stored
  float data[size];

  // aliases to access x, y, z and w co-ordinates
  float& x; // data[0]
  float& y; // data[1]
  float& z; // data[2]
  float& w; // data[3]

  // homogenize a Point3Df p with 4th coord wval
  static Point4Df homogenize(const Point3Df& p, float wval);

  // dehomogenize a Point4Df: if w != 0, return (x/w, y/w, z/w)
  Point3Df dehomogenize() const;

  // number of coordinates
  int length(void) const;

  // sum of coordinates
  float sum() const;

  // dot product
  float dot(const Point4Df& v2) const;
  
  // compute length of vector (distance to origin)
  float norm() const;

  // normalize vector: divide self by self.norm()
  Point4Df& normalize();

  //co-ordinate-wise add,subtract,multiply to self
  Point4Df& operator+=(const Point4Df& other);
  Point4Df& operator-=(const Point4Df& other);
  Point4Df& operator*=(const Point4Df& other);
  
  // add, subtract, multiply, every element by scalar
  Point4Df& operator+=(const float scalar);
  Point4Df& operator-=(const float scalar);
  Point4Df& operator*=(const float scalar);
  
  //co-ordinate-wise addition, subtraction and multiplication
  Point4Df operator+(const Point4Df& v2) const;
  Point4Df operator-(const Point4Df& v2) const;
  Point4Df operator*(const Point4Df& v2) const;
  
  // operators for addition, subtraction, multiplication by scalar
  // always put point on the left
  Point4Df operator*(const float x) const;
  Point4Df operator+(const float x) const;
  Point4Df operator-(const float x) const;

  // Transformations of points: self = t*self
  Point4Df& applyToSelf(const Transform4Df& t);

  // Transformations of points: returns t*self
  Point4Df apply(const Transform4Df& t) const;

  // read point from stream
  std::istream& in(std::istream& is);
  
// append point to end of stream
  std::ostream& out(std::ostream&) const;
};

// read a point:  a binary operator
std::istream& operator>>(std::istream& is, Point4Df& p);

// print a point: a binary operator
std::ostream& operator<<(std::ostream& o, const Point4Df& p);

#endif POINT4DF_H




