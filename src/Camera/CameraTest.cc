// This may look like C code, but it is really -*- C++ -*-

#include <cmath>
#include "Camera.h"
#include "gtest/gtest.h"

int main() {
  Camera c1;
  std::cout << c1 <<std::endl;
  Camera c2(c1);
  std::cout << c2 <<std::endl;
  Camera c3(Point4Dd(1,0,0,1), Point4Dd(0,0,0,1),Point4Dd(0,0,1,0),
				   asin(1), 1, 10.0, 20.0);
  std::cout << c3 <<std::endl;
  std::cout << c3.cameraToWorld <<std::endl;
  c3.setSampleDims(20,15);
  for(int r=0;r<20;r++)
    for(int c=0;c<15;c++)
      std::cout << "Ray = " <<  c3.getRay(r,c) <<std::endl;
  return 0;
}

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

TEST_F(CameraTest,getRayConsistent) {
  for(int r=0; r<20; r++) {
    for(int c=0; c<20; c++) {
      EXPECT_TRUE( cam.getRay(r,c) == cam2.getRay(r,c) );
    }
  }
}
