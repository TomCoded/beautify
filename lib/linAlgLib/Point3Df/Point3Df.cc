// This may look like C code, but it is really -*- C++ -*-

// Point3Df.cc

// (C) 2002 Bill Lenhart

#include <stdlib.h>
#include <cmath>
#include "Point3Df/Point3Df.h"
#include "Transform3Df/Transform3Df.h"

//////////////////////
// class Point3Df //
//////////////////////

// Constructors
// Initialize to (0,0,0)
Point3Df::Point3Df()
: x(data[0]), y(data[1]), z(data[2])
{
  x = 0;
  y = 0;
  z = 0;
}

// Initialize to (xval, yval, zval)
Point3Df::Point3Df(float xval, float yval, float zval)
: x(data[0]), y(data[1]), z(data[2])
{
  x = xval;
  y = yval;
  z = zval;
}

// copy constructor
Point3Df::Point3Df(const Point3Df& other)
: x(data[0]), y(data[1]), z(data[2])
{
  *this = other;
}

// assignment operator
Point3Df& Point3Df::operator=(const Point3Df& other)
{
  if (this != &other) {
    for(int i=0;i<size;i++)
      data[i] = other.data[i];
  }

  return *this;
}

// destructor
Point3Df::~Point3Df() {}

// number of coordinates
int Point3Df::length(void) const
{
  return size;
}

// sum of coordinates
float Point3Df::sum() const
{
  int i;
  float total(0);
  for(i=0;i<size;i++)
    total += data[i];
  return total;
}

// dot product
float Point3Df::dot(const Point3Df& v2) const
{
  Point3Df temp;
  temp = *this;
  temp *= v2;
  return temp.sum();
}

// compute length (distance to origin)
float Point3Df::norm() const
{
  return std::sqrt(this->dot(*this));
}

// normalize: divide self by self.norm()
Point3Df& Point3Df::normalize()
{
  (*this)*=1/this->norm();
  return *this;
}

// compute cross product self x other
Point3Df Point3Df::cross(const Point3Df& other) const
{
  Point3Df result(this->y*other.z - this->z*other.y,
		 - this->x*other.z + this->z*other.x,
		 this->x*other.y - this->y*other.x);
  return result;
}

//co-ordinate-wise add,subtract,multiply to self
Point3Df& Point3Df::operator+=(const Point3Df& other)
{
  for (int i=0;i<size;i++)
    data[i] += other.data[i];
  return *this;
}
  
Point3Df& Point3Df::operator-=(const Point3Df& other)
{
  for (int i=0;i<size;i++)
    data[i] -= other.data[i];
  return *this;
}
  
Point3Df& Point3Df::operator*=(const Point3Df& other)
{
  for (int i=0;i<size;i++)
    data[i] *= other.data[i];
  return *this;
}
  
// add, subtract, multiply, every element by scalar
Point3Df& Point3Df::operator+=(const float scalar)
{
  for (int i=0;i<size;i++)
    data[i] += scalar;
  return *this;
}
  
Point3Df& Point3Df::operator-=(const float scalar)
{
  for (int i=0;i<size;i++)
    data[i] -= scalar;
  return *this;
}
  
Point3Df& Point3Df::operator*=(const float scalar)
{
  for (int i=0;i<size;i++)
    data[i] *= scalar;
  return *this;
}

//co-ordinate-wise addition, subtraction and multiplication
Point3Df Point3Df::operator+(const Point3Df& v2) const
{
  Point3Df temp;
  temp = *this;
  temp += v2;
  return temp;
}

Point3Df Point3Df::operator-(const Point3Df& v2) const
{
  Point3Df temp;
  temp = *this;
  temp -= v2;
  return temp;
}

Point3Df Point3Df::operator*(const Point3Df& v2) const
{
  Point3Df temp;
  temp = *this;
  temp *= v2;
  return temp;
}

// operators for addition, subtraction, multiplication by scalar
// always put vector on the left
Point3Df Point3Df::operator*(const float scalar) const
{
  Point3Df temp;
  temp = *this;
  temp *= scalar;
  return temp;
}

Point3Df Point3Df::operator+(const float scalar) const
{
  Point3Df temp;
  temp = *this;
  temp += scalar;
  return temp;
}

Point3Df Point3Df::operator-(const float scalar) const
{
  Point3Df temp;
  temp = *this;
  temp -= scalar;
  return temp;
}
  
// Transformations of points: self = t*self
Point3Df& Point3Df::applyToSelf(const Transform3Df& t)
{
  *this=t*(*this);
  return *this;
}

// Transformations of points: returns t*self
Point3Df Point3Df::apply(const Transform3Df& t) const
{
  return t*(*this);
}

// append point to end of stream
ostream& Point3Df::out(ostream& o) const
{
  o << "(";
  for(int i=0;i<size-1;i++)
    o << data[i] << ", ";
  o << data[size-1] << ")";

  return o;
}

// read array from stream
istream& Point3Df::in(istream& is)
{
  char c;

  is >> c;
  if (c != '(') {
    cout << "Bad format for Point3Df" << endl;
    exit(1);
  }
  else {
    for(int i=0;i<size-1;i++) {
      is >> data[i] >> c;
      if (c != ',') {
	cout << "Bad format for Point3Df" << endl;
	exit(1);
      }
    }
    is >> data[size-1] >> c;
    if (c != ')') {
      cout << "Bad format for Point3Df" << endl;
      exit(1);
    }
  }

  return is;
}

// read a point:  a binary operator
istream& operator>>(istream& is, Point3Df& p)
{
  p.in(is);
}

// print a point: a binary operator
ostream& operator<<(ostream& o, const Point3Df& p)
{
  return p.out(o);
}

