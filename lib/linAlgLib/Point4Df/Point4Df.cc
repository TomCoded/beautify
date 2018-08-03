// This may look like C code, but it is really -*- C++ -*-

// Point4Df.cc

// (C) 2002 Bill Lenhart

#include <stdlib.h>
#include <cmath>
#include "Point3Df/Point3Df.h"
#include "Point4Df/Point4Df.h"
#include "Transform4Df/Transform4Df.h"

//////////////////////
// class Point4Df //
//////////////////////

// Constructors
// Initialize to (0,0,0,0)
Point4Df::Point4Df()
  : x(data[0]), y(data[1]), z(data[2]), w(data[3])
{
  x = 0;
  y = 0;
  z = 0;
  w = 0;
}

// Initialize to (xval, yval, zval,wval)
Point4Df::Point4Df(float xval, float yval, float zval, float wval)
  : x(data[0]), y(data[1]), z(data[2]), w(data[3])
{
  x = xval;
  y = yval;
  z = zval;
  w = wval;
}

// Initialize to (Point3Df p, wval)
Point4Df::Point4Df(const Point3Df& p, float wval)
  : x(data[0]), y(data[1]), z(data[2]), w(data[3])
{
  x = p.x;
  y = p.y;
  z = p.z;
  w = wval;
}

// copy constructor
Point4Df::Point4Df(const Point4Df& other)
  : x(data[0]), y(data[1]), z(data[2]), w(data[3])
{
  *this = other;
}

// assignment operator
Point4Df& Point4Df::operator=(const Point4Df& other)
{
  if (this != &other) {
    for(int i=0;i<size;i++)
      data[i] = other.data[i];
  }

  return *this;
}

// destructor
Point4Df::~Point4Df() {}

// homogenize a Point3Df p with 4th coord wval
Point4Df Point4Df::homogenize(const Point3Df& p, float wval)
{
  return Point4Df(p.x,p.y,p.z,wval);
}

// dehomogenize a Point4Df: if w != 0, return (x/w, y/w, z/w)
Point3Df Point4Df::dehomogenize() const
{
  if(w != 0)
    return Point3Df(x/w,y/w,z/w);
  else
    return Point3Df(x,y,z);
}

// number of coordinates
int Point4Df::length(void) const
{
  return size;
}

// sum of coordinates
float Point4Df::sum() const
{
  float total(0);
  for(int i=0;i<size;i++)
    total += data[i];
  return total;
}

// dot product
float Point4Df::dot(const Point4Df& v2) const
{
  Point4Df temp;
  temp = *this;
  temp *= v2;
  return temp.sum();
}

// compute length of vector (distance to origin)
float Point4Df::norm() const
{
  return std::sqrt(this->dot(*this));
}

// normalize vector: divide self by self.norm()
Point4Df& Point4Df::normalize()
{
  (*this)*=1/this->norm();
  return *this;
}

//co-ordinate-wise add,subtract,multiply to self
Point4Df& Point4Df::operator+=(const Point4Df& other)
{
  for (int i=0;i<size;i++)
    data[i] += other.data[i];
  return *this;
}
  
Point4Df& Point4Df::operator-=(const Point4Df& other)
{
  for (int i=0;i<size;i++)
    data[i] -= other.data[i];
  return *this;
}
  
Point4Df& Point4Df::operator*=(const Point4Df& other)
{
  for (int i=0;i<size;i++)
    data[i] *= other.data[i];
  return *this;
}
  
// add, subtract, multiply, every element by scalar
Point4Df& Point4Df::operator+=(const float scalar)
{
  for (int i=0;i<size;i++)
    data[i] += scalar;
  return *this;
}
  
Point4Df& Point4Df::operator-=(const float scalar)
{
  for (int i=0;i<size;i++)
    data[i] -= scalar;
  return *this;
}
  
Point4Df& Point4Df::operator*=(const float scalar)
{
  for (int i=0;i<size;i++)
    data[i] *= scalar;
  return *this;
}

//co-ordinate-wise addition, subtraction and multiplication
Point4Df Point4Df::operator+(const Point4Df& v2) const
{
  Point4Df temp;
  temp = *this;
  temp += v2;
  return temp;
}

Point4Df Point4Df::operator-(const Point4Df& v2) const
{
  Point4Df temp;
  temp = *this;
  temp -= v2;
  return temp;
}

Point4Df Point4Df::operator*(const Point4Df& v2) const
{
  Point4Df temp;
  temp = *this;
  temp *= v2;
  return temp;
}

// operators for addition, subtraction, multiplication by scalar
// always put vector on the left
Point4Df Point4Df::operator*(const float scalar) const
{
  Point4Df temp;
  temp = *this;
  temp *= scalar;
  return temp;
}

Point4Df Point4Df::operator+(const float scalar) const
{
  Point4Df temp;
  temp = *this;
  temp += scalar;
  return temp;
}

Point4Df Point4Df::operator-(const float scalar) const
{
  Point4Df temp;
  temp = *this;
  temp -= scalar;
  return temp;
}
  
// Transformations of points: self = t*self
Point4Df& Point4Df::applyToSelf(const Transform4Df& t)
{
  *this=t*(*this);
  return *this;
}

// Transformations of points: returns t*self
Point4Df Point4Df::apply(const Transform4Df& t) const
{
  return t*(*this);
}

// append point to end of stream
ostream& Point4Df::out(ostream& o) const
{
  o << "(";
  for(int i=0;i<size-1;i++)
    o << data[i] << ", ";
  o << data[size-1] << ")";

  return o;
}

// read array from stream
istream& Point4Df::in(istream& is)
{
  char c;

  is >> c;
  if (c != '(') {
    cout << "Bad format for Point4Df" << endl;
    exit(1);
  }
  else {
    for(int i=0;i<size-1;i++) {
      is >> data[i] >> c;
      if (c != ',') {
	cout << "Bad format for Point4Df" << endl;
	exit(1);
      }
    }
    is >> data[size-1] >> c;
    if (c != ')') {
      cout << "Bad format for Point4Df" << endl;
      exit(1);
    }
  }
  return is;
}

// read a point:  a binary operator
istream& operator>>(istream& is, Point4Df& p)
{
  p.in(is);
}

// print a point: a binary operator
ostream& operator<<(ostream& o, const Point4Df& p)
{
  return p.out(o);
}

