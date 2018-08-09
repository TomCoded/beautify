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

TEST_F(CameraTest,getRayConsistent) {
  for(int r=0; r<20; r++) {
    for(int c=0; c<20; c++) {
      EXPECT_TRUE( cam.getRay(r,c) == cam2.getRay(r,c) );
    }
  }
}
