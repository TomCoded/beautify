// This may look like C code, but it is really -*- C++ -*-

#include <cmath>
#include "Camera.h"
#include "gtest/gtest.h"

namespace {
  class CameraTest : public ::testing::Test {
  protected:
    CameraTest() {
      this->cam.setSampleDims(20,15);
      this->cam2.setSampleDims(20,15);
    }

  Camera cam;
  Camera cam2;
  };
}

TEST_F(CameraTest,defaultConstructor) {
  EXPECT_EQ(cam.eye.x,0.0);
  EXPECT_EQ(cam.eye.y,0.0);
  EXPECT_EQ(cam.eye.z,2.0);
  EXPECT_EQ(cam.eye.data[3],1.0);
  EXPECT_EQ(cam.aspectRatio,1.0);
}

TEST_F(CameraTest,constructor) {
}

TEST_F(CameraTest,copyConstructor) {
}

TEST_F(CameraTest,assignment) {
  cam.setSampleDims(300,300);
  cam2=cam;
  EXPECT_EQ(cam2.rows,300);
  EXPECT_EQ(cam2.cols,300);
}

TEST_F(CameraTest,getRay) {
  for(int r=0; r<20; r++) {
    for(int c=0; c<15; c++) {
      std::shared_ptr<Ray> r1 = cam.getRay(r,c);
      EXPECT_TRUE( *r1 == *cam2.getRay(r,c) );
      EXPECT_TRUE( r1->dir.z == -1 );
      if(c<7) {
	EXPECT_LT( r1->dir.x, 0.0 );
      }
    }
  }
}

TEST_F(CameraTest,setSampleDims) {
  for(int r=50; r<3000; r+=150) {
    for (int c=50; c<2000; c+=150) {
      cam.setSampleDims(r,c);
      EXPECT_EQ( cam.rows,r );
      EXPECT_EQ( cam.cols,c );
    }
  }
}

TEST_F(CameraTest,slide) {
  //slide camera with doubles

  //slide camera with Point4Dd
}

TEST_F(CameraTest,rotate) {
}

TEST_F(CameraTest,out) {
}

TEST_F(CameraTest,in) {
}

TEST_F(CameraTest,setCameraCoordSys) {
}

TEST_F(CameraTest,resetCameraTransform) {
}

TEST_F(CameraTest,resetCameraCoordSys) {
}

TEST_F(CameraTest,funIn) {
}

TEST_F(CameraTest,InViewVol) {
}

TEST_F(CameraTest,setTime) {
}


TEST_F(CameraTest,getRayConsistent) {
  for(int r=0; r<20; r++) {
    for(int c=0; c<15; c++) {
      std::shared_ptr<Ray> r1 = cam.getRay(r,c);
      std::shared_ptr<Ray> r2 = cam2.getRay(r,c);
      EXPECT_EQ( *r1, *r2 );
    }
  }
}
