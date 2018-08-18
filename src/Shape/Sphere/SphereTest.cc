// This may look like C code, but it is really -*- C++ -*-

#include <cmath>
#include "Sphere.h"
#include "gtest/gtest.h"

namespace {
  class SphereTest : public ::testing::Test {
  protected:
    SphereTest() {
    }
  };
}

TEST_F(SphereTest,defaultConstructor) {
  Sphere sphere;
  EXPECT_EQ(sphere.GetRadius(),1.0);
}

TEST_F(SphereTest,constructor) {
  for(int i=1.5; i<100.6; i++) {
    Point3Dd center(i,i,i);
    Sphere sphere(center, i);
    EXPECT_EQ(sphere.GetRadius(),i);
  }
}

TEST_F(SphereTest,copyConstructor) {
}

TEST_F(SphereTest,implicit) {
  for(int i=1.5; i<100.6; i++) {
    Point3Dd center(i,i,i);
    Sphere sphere(center, i);
    double x = i/10;
    double y = drand48()*(9.0/10.0);
    double z = sqrt(i*i-(x*x+y*y));
    Point3Dd onSphere(x,y,z);
    EXPECT_LT(sphere.implicit(onSphere),0.01);
    EXPECT_GT(sphere.implicit(onSphere),-0.01);
  }
}

TEST_F(SphereTest,closestIntersect) {
  Ray r(100.0,0.0,0.0,
	-1.0,0.0,0.0);
  for(int i=1.5; i<300.6; i++) {
    Point3Dd center(0,0,0);
    Sphere sphere(center, i);
    double closestI = sphere.closestIntersect(r);
    if(i<99) {
      EXPECT_GT(closestI,1.0);
      EXPECT_LT(closestI,100.0);
    }
    if(i>101) {
      EXPECT_GT(closestI,100.0);
      EXPECT_LT(closestI,450.0);
    }
  }
}

TEST_F(SphereTest,getNormal) {
  Ray r(100.0,0.0,0.0,
	-1.0,0.0,0.0);
  for(int i=1.5; i<300.6; i++) {
    Point3Dd center(0,0,0);
    Sphere sphere(center, i);
    Ray normal = sphere.getNormal(r);
    if(i<=100) { 
      EXPECT_GT(normal.dir.x,0.99);
      EXPECT_LT(normal.dir.x,1.01);
      EXPECT_GT(normal.dir.y,-0.01);
      EXPECT_LT(normal.dir.y,0.01);
      EXPECT_GT(normal.dir.z,-0.01);
      EXPECT_LT(normal.dir.z,0.01);
    } else {
      EXPECT_GT(normal.dir.x,-1.01);
      EXPECT_LT(normal.dir.x,-0.99);
      EXPECT_GT(normal.dir.y,-0.01);
      EXPECT_LT(normal.dir.y,0.01);
      EXPECT_GT(normal.dir.z,-0.01);
      EXPECT_LT(normal.dir.z,0.01);
    }
  }

}
