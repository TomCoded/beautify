#include "Photon.h"
#include "gtest/gtest.h"
#include <math.h>

#include <string>
#include <sstream>

namespace {

  class photonTest : public ::testing::Test {
  protected:
    
    photonTest() {
      // set-up for tests
    }

    ~photonTest() override {
      // clean-up that doesn't throw exceptions
    }

    void SetUp() override {
     // Code here will be called immediately after the constructor (right
     // before each test).
    }

    void TearDown() override {
      // Code here will be called immediately after each test (right
      // before the destructor).
    }
    
    // Objects declared here can be used by all tests in the test case
  };
  
};

TEST_F(photonTest,photonInWorks) {
  std::string testString("");
  float nineFloats[9];
  for(int i=0; i<9; i++) {
    nineFloats[i] = round(drand48()*100);
    testString =
      testString+
      std::to_string(nineFloats[i])+
      std::string(" ");
  }
  int offset = (int)round((drand48() * 1000000));
  short flag = (short int)round((drand48() * 20000));
  short bounced = (short int)round((drand48() * 20000));
  testString += std::to_string(flag);
  testString += std::string(" ");
  testString += std::to_string(bounced);
  testString += std::string(" ");
  testString += std::to_string(offset);
  
  Photon * p1 = new Photon();
  std::istringstream testStream=std::istringstream(testString);
  testStream >> *p1;

  EXPECT_EQ( p1->x, nineFloats[0] );
  EXPECT_EQ( p1->y, nineFloats[1] );
  EXPECT_EQ( p1->z, nineFloats[2] );
  for(int i=0; i<9; i++) {
    EXPECT_EQ( ((float*)(p1))[i], nineFloats[i] );
  }
  EXPECT_EQ( p1->flag, flag );
  EXPECT_EQ( p1->bounced, bounced );
  EXPECT_EQ( p1->offset, offset );
}


