// This may look like C code, but it is really -*- C++ -*-

// Point4Dd.cc

// (C) 2002 Bill Lenhart

#include <stdlib.h>
#include <cmath>
#include "Point3Dd/Point3Dd.h"
#include "Point4Dd/Point4Dd.h"
#include "Transform4Dd/Transform4Dd.h"

//////////////////////
// class Point4Dd //
//////////////////////

// Constructors
// Initialize to (0,0,0,0)
Point4Dd::Point4Dd()
  : x(data[0]), y(data[1]), z(data[2]), w(data[3])
{
  x = 0;
  y = 0;
  z = 0;
  w = 0;
}

// Initialize to (xval, yval, zval,wval)
Point4Dd::Point4Dd(double xval, double yval, double zval, double wval)
  : x(data[0]), y(data[1]), z(data[2]), w(data[3])
{
  x = xval;
  y = yval;
  z = zval;
  w = wval;
}

// Initialize to (Point3Dd p, wval)
Point4Dd::Point4Dd(const Point3Dd& p, double wval)
  : x(data[0]), y(data[1]), z(data[2]), w(data[3])
{
  x = p.x;
  y = p.y;
  z = p.z;
  w = wval;
}

// copy constructor
Point4Dd::Point4Dd(const Point4Dd& other)
  : x(data[0]), y(data[1]), z(data[2]), w(data[3])
{
  *this = other;
}

// assignment operator
Point4Dd& Point4Dd::operator=(const Point4Dd& other)
{
  if (this != &other) {
    for(int i=0;i<size;i++)
      data[i] = other.data[i];
  }

  return *this;
}

// destructor
Point4Dd::~Point4Dd() {}

// homogenize a Point3Dd p with 4th coord wval
Point4Dd Point4Dd::homogenize(const Point3Dd& p, double wval)
{
  return Point4Dd(p.x,p.y,p.z,wval);
}

// dehomogenize a Point4Dd: if w != 0, return (x/w, y/w, z/w)
Point3Dd Point4Dd::dehomogenize() const
{
  if(w != 0)
    return Point3Dd(x/w,y/w,z/w);
  else
    return Point3Dd(x,y,z);
}

// number of coordinates
int Point4Dd::length(void) const
{
  return size;
}

// sum of coordinates
double Point4Dd::sum() const
{
  double total(0);
  for(int i=0;i<size;i++)
    total += data[i];
  return total;
}

// dot product
double Point4Dd::dot(const Point4Dd& v2) const
{
  Point4Dd temp;
  temp = *this;
  temp *= v2;
  return temp.sum();
}

// compute length of vector (distance to origin)
double Point4Dd::norm() const
{
  return std::sqrt(this->dot(*this));
}

// normalize vector: divide self by self.norm()
Point4Dd& Point4Dd::normalize()
{
  (*this)*=1/this->norm();
  return *this;
}

//co-ordinate-wise add,subtract,multiply to self
Point4Dd& Point4Dd::operator+=(const Point4Dd& other)
{
  for (int i=0;i<size;i++)
    data[i] += other.data[i];
  return *this;
}
  
Point4Dd& Point4Dd::operator-=(const Point4Dd& other)
{
  for (int i=0;i<size;i++)
    data[i] -= other.data[i];
  return *this;
}
  
Point4Dd& Point4Dd::operator*=(const Point4Dd& other)
{
  for (int i=0;i<size;i++)
    data[i] *= other.data[i];
  return *this;
}
  
// add, subtract, multiply, every element by scalar
Point4Dd& Point4Dd::operator+=(const double scalar)
{
  for (int i=0;i<size;i++)
    data[i] += scalar;
  return *this;
}
  
Point4Dd& Point4Dd::operator-=(const double scalar)
{
  for (int i=0;i<size;i++)
    data[i] -= scalar;
  return *this;
}
  
Point4Dd& Point4Dd::operator*=(const double scalar)
{
  for (int i=0;i<size;i++)
    data[i] *= scalar;
  return *this;
}

//co-ordinate-wise addition, subtraction and multiplication
Point4Dd Point4Dd::operator+(const Point4Dd& v2) const
{
  Point4Dd temp;
  temp = *this;
  temp += v2;
  return temp;
}

Point4Dd Point4Dd::operator-(const Point4Dd& v2) const
{
  Point4Dd temp;
  temp = *this;
  temp -= v2;
  return temp;
}

Point4Dd Point4Dd::operator*(const Point4Dd& v2) const
{
  Point4Dd temp;
  temp = *this;
  temp *= v2;
  return temp;
}

// operators for addition, subtraction, multiplication by scalar
// always put vector on the left
Point4Dd Point4Dd::operator*(const double scalar) const
{
  Point4Dd temp;
  temp = *this;
  temp *= scalar;
  return temp;
}

Point4Dd Point4Dd::operator+(const double scalar) const
{
  Point4Dd temp;
  temp = *this;
  temp += scalar;
  return temp;
}

Point4Dd Point4Dd::operator-(const double scalar) const
{
  Point4Dd temp;
  temp = *this;
  temp -= scalar;
  return temp;
}
  
// Transformations of points: self = t*self
Point4Dd& Point4Dd::applyToSelf(const Transform4Dd& t)
{
  *this=t*(*this);
  return *this;
}

// Transformations of points: returns t*self
Point4Dd Point4Dd::apply(const Transform4Dd& t) const
{
  return t*(*this);
}

// append point to end of stream
ostream& Point4Dd::out(ostream& o) const
{
  o << "(";
  for(int i=0;i<size-1;i++)
    o << data[i] << ", ";
  o << data[size-1] << ")";

  return o;
}

// read array from stream
istream& Point4Dd::in(istream& is)
{
  char c;

  is >> c;
  if (c != '(') {
    cout << "Bad format for Point4Dd" << endl;
    exit(1);
  }
  else {
    for(int i=0;i<size-1;i++) {
      is >> data[i] >> c;
      if (c != ',') {
	cout << "Bad format for Point4Dd" << endl;
	exit(1);
      }
    }
    is >> data[size-1] >> c;
    if (c != ')') {
      cout << "Bad format for Point4Dd" << endl;
      exit(1);
    }
  }
  return is;
}

// read a point:  a binary operator
istream& operator>>(istream& is, Point4Dd& p)
{
  p.in(is);
}

// print a point: a binary operator
ostream& operator<<(ostream& o, const Point4Dd& p)
{
  return p.out(o);
}

