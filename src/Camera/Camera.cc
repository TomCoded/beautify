// This may look like C code, but it is really -*- C++ -*-

#include <cmath>
#include "Defs.h"
#include "Camera.h"
#include "Point3Dd.h"
#include "../TransformMaker/TransformMaker.h"
#include "FunNode.h"
#include "SumFunNode.h"
#include "MultFunNode.h"
#include "NumFunNode.h"

//  Create a Camera and set all entries to default values
Camera::Camera() : eye(0.0, 0.0, 2.0, 1.0), origEye(0.0, 0.0, 2.0, 1.0), 
		   lookAt(0.0, 0.0, 0.0, 1.0), up(0.0, 1.0, 0.0, 0.0),
		   viewAngle(asin(1)),aspectRatio(1.0),
		   near(-1.0), far(-100.0),
		   funViewAngle(0), funAspectRatio(0),
		   funNear(0), funFar(0), funEye(0),
		   funUp(0), funLookAt(0)
{
  wld2Camgl = new double[16];
  setCameraCoordSys();  
}

//Create a Camera using given parameters: Does NOT make copies!
//Points should be in 3D coordinates
Camera::Camera(const Point4Dd& camEye, const Point4Dd& camLookAt, 
	       const Point4Dd& camUp,
	       double camViewAngle, double camAspectRatio,
	       double camNear, double camFar) :
  eye(camEye), origEye(camEye), lookAt(camLookAt), up(camUp),
  viewAngle(camViewAngle),aspectRatio(camAspectRatio),
  near(-camNear), far(-camFar),
  funViewAngle(0), funAspectRatio(0),
  funNear(0), funFar(0), funEye(0), funUp(0), funLookAt(0)
{
  wld2Camgl = new double[16];
  setCameraCoordSys();  
}

// copy constructor
Camera::Camera(const Camera& other) {
  *this = other;
}
	
// assignment operator
Camera& Camera::operator=(const Camera& other) {
  if(this != &other) {
    wld2Camgl = new double[16];
    eye = other.eye;
    origEye = other.origEye;
    lookAt = other.lookAt;
    up = other.up;
    viewAngle = other.viewAngle;
    aspectRatio = other.aspectRatio;
    near = other.near;
    far = other.far;
    u = other.u;
    v = other.v;
    n = other.n;
    cameraToWorld = other.cameraToWorld;
    worldToCamera = other.worldToCamera;
    left = other.left;
    right = other.right;
    bottom = other.bottom;
    top = other.top;

    rows = other.rows;
    cols = other.cols;
    
    if(other.funViewAngle) {
      funViewAngle = other.funViewAngle->clone();
      funAspectRatio = other.funAspectRatio->clone();
      funNear = other.funNear->clone();
      funFar = other.funFar->clone();
      funEye = new FunPoint4Dd(*other.funEye);
      funUp = new FunPoint4Dd(*other.funUp);
      funLookAt = new FunPoint4Dd(*other.funLookAt);
    } else {
      funViewAngle = funAspectRatio
	= funNear = funFar = 0;
      funEye = funUp = funLookAt
	= 0;
    }

    for(int i=0;i<16;i++)
      wld2Camgl[i]=other.wld2Camgl[i];
  }
  return *this;
}

Camera::~Camera() {
  delete [] wld2Camgl;
  delete funEye, funUp, funLookAt;
  delete funViewAngle,
    funAspectRatio,
    funNear,
    funFar;
}

//update camera for current world time
void Camera::setTime(double t) {
  if(funViewAngle) {
    //if the camera moves
    eye    = funEye    ->eval(t);
    up     = funUp     ->eval(t);
    lookAt = funLookAt ->eval(t);
    viewAngle   = funViewAngle    ->eval(t);
    aspectRatio = funAspectRatio  ->eval(t);
    near        = funNear         ->eval(t);
    far         = funFar          ->eval(t);
    setCameraCoordSys();
  }
}

// Camera manipulation methods

// Set dimensions of sample grid
void Camera::setSampleDims(int r, int c) {
  rows = r;
  cols = c;
}

// Get sample ray
Ray Camera::getRay(int r, int c) {
  // Build ray by adding its u, v, and n components
  // Make the direction std::vector extend from eye to grid point
  // to give a ray with t=1 corresponding to grid point
  Point4Dd dir(0,0,-1,0);
  //want 0 to be at left, and cols-1 to be at right for symmetry
  dir.x+=(left+(c)*(right-left)/(cols-1));
  dir.y+=(bottom+(r)*(top-bottom)/(rows-1));
  Ray rv(Point4Dd(0,0,0,1),dir);
  rv.applyToSelf(cameraToWorld);
  return rv;
}

//returns true iff InViewVol is inside the view volume of the camera
//InViewVol is in world coordinates
bool Camera::InViewVol(const Point4Dd& p) const {
  //localp is the direction to the point in local coords
  //from the eye, at our local origin.
  Point4Dd localp = worldToCamera*p;
  
  //reject out of hand if behind eye
  if(localp.z > 0)
    return false;

  //distance to point
  double distToPoint = localp.norm();

  //cosine of angle between n-axis and ray through point
  double cosTheta = 
    n.dot(localp) / (n.norm() * distToPoint );
  
  //closer than near plane
  if(distToPoint <= near / cosTheta)
    return false;

  //farther away than far plane
  //>=
  if(distToPoint < far / cosTheta)
    return false;
  
  return true;
}

// Compute camera transform from eye, u, v, and n
void Camera::resetCameraTransform() {
  cameraToWorld = Transform4Dd(u, v, n, eye);
  worldToCamera = Transform4Dd(u.x, u.y, u.z, -eye.dot(u),
			       v.x, v.y, v.z, -eye.dot(v),
			       n.x, n.y, n.z, -eye.dot(n),
			       0, 0, 0, 1);
  int i = 0;
  for(int c=0;c<4;c++)
    for(int r=0;r<4;r++)
      wld2Camgl[i++] = worldToCamera.data[r][c];
}

// Compute camera coordinate frame from eye, n and up PtVec's
void Camera::setCameraCoordSys() {
  n=eye-lookAt;
  n.normalize();
  u=Point4Dd(up.dehomogenize().cross(n.dehomogenize()),0);
  u.normalize();
  v=Point4Dd(n.dehomogenize().cross(u.dehomogenize()),0);
  resetCameraTransform();
  top = tan(viewAngle/2);
  bottom = -top;
  right = aspectRatio*(top-bottom)/2;
  left = -right;
}

// Reset camera to original values
void Camera::resetCameraCoordSys() {
  eye = origEye;
  setCameraCoordSys();
}

//  Slide camera using direction std::vector
void Camera::slide(const Point4Dd& dir) {
  eye+=dir;
  resetCameraTransform();
}

//  Slide camera using direction std::vector
void Camera::slide(double du, double dv, double dn) {
  slide(Point4Dd(du,dv,dn,0));
}

// Rotate camera about direction by angle (in degrees
void Camera::rotate(double angle, const Point4Dd& dir) {
  Transform4Dd R = MakeRotation(angle, dir.dehomogenize());
  u.applyToSelf(R);
  v.applyToSelf(R);
  n.applyToSelf(R);
  resetCameraTransform();
}

// Rotate camera about direction by angle (in degrees
void Camera::rotate(double angle, double du, double dv, double dn) {
  rotate(angle, Point4Dd(du,dv,dn,0));
}

// append camera to stream
std::ostream& Camera::out(std::ostream& os) const {
  os << "(" << eye.dehomogenize() << "," << lookAt.dehomogenize() << ",";
  os << up.dehomogenize() << ",";
  os << viewAngle << "," << aspectRatio << "," << near << "," << far;
  os << ")";
  return os;
}

// read camera from stream
// skips leading and internal whitespace
std::istream& Camera::in(std::istream& is) {
  char c;
  Point3Dd inputPt;
  std::string formatErr("Bad Camera Format");
  is >> c;
  FORMATTEST(c,'(',formatErr)
  is >> inputPt >> c;
  eye = Point4Dd(inputPt,1);
  FORMATTEST(c,',',formatErr)
  is >> inputPt >> c;
  lookAt = Point4Dd(inputPt,1);
  FORMATTEST(c,',',formatErr)
  is >> inputPt >> c;
  up = Point4Dd(inputPt,1);
  FORMATTEST(c,',',formatErr)
  is >> viewAngle >> c;
  FORMATTEST(c,',',formatErr)
  is >> aspectRatio >> c;
  FORMATTEST(c,',',formatErr)
  is >> near >> c;
  near*=-1;
  FORMATTEST(c,',',formatErr)
  is >> far >> c;
  far*=-1;
  FORMATTEST(c,')',formatErr)
  setCameraCoordSys();
  origEye=eye;
  return is;
}

// read Function camera from stream
// skips leading and internal whitespace
std::istream& Camera::funIn(std::istream& is) {
  char c;
  Point3Dd inputPt;
  SumFunNode parser;

  if(!funViewAngle) {
    //allocate space for functions
    funEye = new FunPoint4Dd();
    funUp = new FunPoint4Dd();
    funLookAt = new FunPoint4Dd();
  }

  std::string formatErr("Bad format for Camera.");
  is >> c;
  FORMATTEST(c,'(',formatErr)
  is >> *funEye >> c;
  FORMATTEST(c,',',formatErr)
  is >> *funLookAt >> c;
  FORMATTEST(c,',',formatErr)
  is >> *funUp >> c;
  up = Point4Dd(inputPt,1);
  FORMATTEST(c,',',formatErr)
  funViewAngle = parser.in(is);
  is >> c;
  FORMATTEST(c,',',formatErr)
  funAspectRatio = parser.in(is);
  is >> c;
  FORMATTEST(c,',',formatErr)
  funNear = new MultFunNode(new NumFunNode(-1.0),
			    parser.in(is));
  is >> c;
  FORMATTEST(c,',',formatErr)
  funFar = new MultFunNode(new NumFunNode(-1.0),
			   parser.in(is));
  is >> c;
  FORMATTEST(c,')',formatErr)
  setTime(0);
  origEye=eye;
  return is;
}

// Non-member functions for the type

// read Camera from stream
std::istream& operator>>(std::istream & is, Camera& c) {
  return c.in(is);
}

// write Camera to stream
std::ostream& operator<<(std::ostream & os, const Camera& c) {
  return c.out(os);
}

