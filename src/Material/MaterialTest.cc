#include "Material.h"
#include <Photon.h>
#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include <sstream>
#include "Defs.h"

namespace {
  class MaterialTest : public ::testing::Test {
  protected:
    MaterialTest() {
      mat = new Material();
      for(int i=0; i<9; i++) {
	((float *)&samplePhoton)[i]=1.0;
      }
      samplePhoton.offset=0;
      samplePhoton.flag=0;
    }

    ~MaterialTest() override {
      delete mat;
    }

    Material * mat;
    Photon samplePhoton;
  };
}

TEST_F(MaterialTest,defaultConstructor) {
}

TEST_F(MaterialTest,copyConstructor) {
}

TEST_F(MaterialTest,clone) {
}

TEST_F(MaterialTest,participates) {
}

TEST_F(MaterialTest,nRoulette) {
  std::string testString("((0.329412,0.223529,0.027451),(0.780392,0.568627,0.113725),(0.992157,0.941176,0.807843),27.8974)");
  std::istringstream testStream(testString);
  mat->in(testStream);
  srand48(24);
  int roulletteResult = mat->nRoulette(samplePhoton);
  EXPECT_EQ(roulletteResult, DIFFUSE_REFLECTION);
  EXPECT_GT(samplePhoton.r,1.7);
  EXPECT_LT(samplePhoton.r,1.8);
  EXPECT_GT(samplePhoton.g,1.2);
  EXPECT_LT(samplePhoton.g,1.3);
  EXPECT_GT(samplePhoton.b,0.2);
  EXPECT_LT(samplePhoton.b,0.3);
}

TEST_F(MaterialTest, DoBRDF) {
}

TEST_F(MaterialTest, DoSpecBRDF) {
}

TEST_F(MaterialTest, getSpecCo) {
}

TEST_F(MaterialTest, in) {
  std::string testString("((0.329412,0.223529,0.027451),(0.780392,0.568627,0.113725),(0.992157,0.941176,0.807843),27.8974)");
  std::istringstream testStream(testString);
  mat->in(testStream);
  srand48(24);
  int roulletteResult = mat->nRoulette(samplePhoton);
  EXPECT_EQ(roulletteResult, DIFFUSE_REFLECTION);
  EXPECT_GT(samplePhoton.r,1.7);
  EXPECT_LT(samplePhoton.r,1.8);
  EXPECT_GT(samplePhoton.g,1.2);
  EXPECT_LT(samplePhoton.g,1.3);
  EXPECT_GT(samplePhoton.b,0.2);
  EXPECT_LT(samplePhoton.b,0.3);
}

TEST_F(MaterialTest, out) {
  std::string testString("((0.329412, 0.223529, 0.027451),(0.780392, 0.568627, 0.113725),(0.992157, 0.941176, 0.807843),27.8974)");
  std::istringstream testStream(testString);
  mat->in(testStream);
  std::ostringstream outStream;
  std::ostream *oStreamOut = (std::ostream*)&outStream;
  mat->out(*oStreamOut);
  std::string output;
  output = outStream.str();
  EXPECT_EQ(output,testString);
}
