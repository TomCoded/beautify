// This may look like C code, but it is really -*- C++ -*-

// Point3Dd.cc

// (C) 2002 Bill Lenhart

#include <stdlib.h>
#include "Point3Dd/Point3Dd.h"
#include "Transform3Dd/Transform3Dd.h"

//////////////////////
// class Point3Dd //
//////////////////////

// Constructors
// Initialize to (0,0,0)
Point3Dd::Point3Dd()
: x(data[0]), y(data[1]), z(data[2])
{
  x = 0;
  y = 0;
  z = 0;
}

// Initialize to (xval, yval, zval)
Point3Dd::Point3Dd(double xval, double yval, double zval)
: x(data[0]), y(data[1]), z(data[2])
{
  x = xval;
  y = yval;
  z = zval;
}

// copy constructor
Point3Dd::Point3Dd(const Point3Dd& other)
: x(data[0]), y(data[1]), z(data[2])
{
  *this = other;
}

// assignment operator
Point3Dd& Point3Dd::operator=(const Point3Dd& other)
{
  if (this != &other) {
    for(int i=0;i<size;i++)
      data[i] = other.data[i];
  }

  return *this;
}

// destructor
Point3Dd::~Point3Dd() {}

// number of coordinates
int Point3Dd::length(void) const
{
  return size;
}

// sum of coordinates
double Point3Dd::sum() const
{
  int i;
  double total(0);
  for(i=0;i<size;i++)
    total += data[i];
  return total;
}

// dot product
double Point3Dd::dot(const Point3Dd& v2) const
{
  Point3Dd temp;
  temp = *this;
  temp *= v2;
  return temp.sum();
}

// compute length (distance to origin)
double Point3Dd::norm() const
{
  return sqrt(this->dot(*this));
}

// normalize: divide self by self.norm()
Point3Dd& Point3Dd::normalize()
{
  (*this)*=1/this->norm();
  return *this;
}

// compute cross product self x other
Point3Dd Point3Dd::cross(const Point3Dd& other) const
{
  Point3Dd result(this->y*other.z - this->z*other.y,
		 - this->x*other.z + this->z*other.x,
		 this->x*other.y - this->y*other.x);
  return result;
}

//co-ordinate-wise add,subtract,multiply to self
Point3Dd& Point3Dd::operator+=(const Point3Dd& other)
{
  for (int i=0;i<size;i++)
    data[i] += other.data[i];
  return *this;
}
  
Point3Dd& Point3Dd::operator-=(const Point3Dd& other)
{
  for (int i=0;i<size;i++)
    data[i] -= other.data[i];
  return *this;
}
  
Point3Dd& Point3Dd::operator*=(const Point3Dd& other)
{
  for (int i=0;i<size;i++)
    data[i] *= other.data[i];
  return *this;
}
  
// add, subtract, multiply, every element by scalar
Point3Dd& Point3Dd::operator+=(const double scalar)
{
  for (int i=0;i<size;i++)
    data[i] += scalar;
  return *this;
}
  
Point3Dd& Point3Dd::operator-=(const double scalar)
{
  for (int i=0;i<size;i++)
    data[i] -= scalar;
  return *this;
}
  
Point3Dd& Point3Dd::operator*=(const double scalar)
{
  for (int i=0;i<size;i++)
    data[i] *= scalar;
  return *this;
}

//co-ordinate-wise addition, subtraction and multiplication
Point3Dd Point3Dd::operator+(const Point3Dd& v2) const
{
  Point3Dd temp;
  temp = *this;
  temp += v2;
  return temp;
}

Point3Dd Point3Dd::operator-(const Point3Dd& v2) const
{
  Point3Dd temp;
  temp = *this;
  temp -= v2;
  return temp;
}

Point3Dd Point3Dd::operator*(const Point3Dd& v2) const
{
  Point3Dd temp;
  temp = *this;
  temp *= v2;
  return temp;
}

// operators for addition, subtraction, multiplication by scalar
// always put vector on the left
Point3Dd Point3Dd::operator*(const double scalar) const
{
  Point3Dd temp;
  temp = *this;
  temp *= scalar;
  return temp;
}

Point3Dd Point3Dd::operator+(const double scalar) const
{
  Point3Dd temp;
  temp = *this;
  temp += scalar;
  return temp;
}

Point3Dd Point3Dd::operator-(const double scalar) const
{
  Point3Dd temp;
  temp = *this;
  temp -= scalar;
  return temp;
}
  
// Transformations of points: self = t*self
Point3Dd& Point3Dd::applyToSelf(const Transform3Dd& t)
{
  *this=t*(*this);
  return *this;
}

// Transformations of points: returns t*self
Point3Dd Point3Dd::apply(const Transform3Dd& t) const
{
  return t*(*this);
}

// append point to end of stream
ostream& Point3Dd::out(ostream& o) const
{
  o << "(";
  for(int i=0;i<size-1;i++)
    o << data[i] << ", ";
  o << data[size-1] << ")";

  return o;
}

// read array from stream
istream& Point3Dd::in(istream& is)
{
  char c;

  is >> c;
  if (c != '(') {
    cout << "Bad format for Point3Dd" << endl;
    exit(1);
  }
  else {
    for(int i=0;i<size-1;i++) {
      is >> data[i] >> c;
      if (c != ',') {
	cout << "Bad format for Point3Dd" << endl;
	exit(1);
      }
    }
    is >> data[size-1] >> c;
    if (c != ')') {
      cout << "Bad format for Point3Dd" << endl;
      exit(1);
    }
  }

  return is;
}

// read a point:  a binary operator
istream& operator>>(istream& is, Point3Dd& p)
{
  p.in(is);
}

// print a point: a binary operator
ostream& operator<<(ostream& o, const Point3Dd& p)
{
  return p.out(o);
}

