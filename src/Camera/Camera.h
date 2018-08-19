// This may look like C code, but it is really -*- C++ -*-

// Camera.h a simple camera object

// (C) Anonymous1 

// Modifications by Tom White for Moving Cameras

#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include "Point4Dd.h"
#include "Transform4Dd.h"
#include "Ray/Ray.h"

#include "FunTransform4Dd.h"
#include "FunPoint4Dd.h"

class Camera {
public:
  // Parameters defining a camera
  Point4Dd eye, origEye, up, lookAt;
  double viewAngle, aspectRatio, near, far;

  // local coordinate system
  Point4Dd u,v,n;

  // Camera to World Coordinates transform
  Transform4Dd cameraToWorld, worldToCamera;
  double * wld2Camgl;

  // u,v bounds for view rectangle
  double left, right, bottom, top;
  
  // Number of rows and columns of sampling rays
  int rows, cols;

  //  Create a Camera and set all entries to default values
  Camera();
	
  //Create a Camera using given parameters: Does NOT make copies!
  //Points should be in 4D coordinates
  Camera(const Point4Dd& camEye, const Point4Dd& camLookAt,
	 const Point4Dd& camUp,
	 double camViewAngle, double camAspectRatio,
	 double camNear, double camFar);

  // copy constructor
  Camera(const Camera& other);
	
  // assignment operator
  Camera& operator=(const Camera& other);

  ~Camera();

  // Camera manipulation methods

  // Set dimensions of sample grid
  void setSampleDims(int r, int c);

  // Get sample ray
  Ray getRay(int r, int c);

  //  Slide camera using direction std::vector
  void slide(double du, double dv, double dn);
  
  //  Slide camera using direction std::vector
  void slide(const Point4Dd& dir);

  // Rotate camera about direction by angle (in degrees
  void rotate(double angle, double du, double dv, double dn);

  // Rotate camera about direction by angle (in degrees
  void rotate(double angle, const Point4Dd& dir);

  // append camera to stream
  std::ostream& out(std::ostream& os) const;

  // read camera from stream
  // skips leading and internal whitespace
  std::istream& in(std::istream& is);

  // Compute camera coordinate frame from eye, lookAt and up
  void setCameraCoordSys();

  // Compute camera matrix from eye, u, v, n
  void resetCameraTransform();

  // Reset camera to original values
  void resetCameraCoordSys();

  //reads temporal dependent camera from stream
  std::istream& funIn(std::istream &is);

  //returns true iff p (World Coords) is within near/far planes
  bool InViewVol(const Point4Dd& p) const;
  
  //Temporally dependent state info

  FunPoint4Dd * funEye, * funUp, * funLookAt;
  FunNode * funViewAngle,
    * funAspectRatio,
    * funNear,
    * funFar;
  
  //updates the state of the camera to reflect its position,
  //etc... at time t
  void setTime(double t);

};

// Non-member functions for the type

// read Camera from stream
std::istream& operator>>(std::istream & is, Camera& c);

// write Camera to stream
std::ostream& operator<<(std::ostream & os, const Camera& c);

#endif CAMERA_H

