#include "Scene.h"
#include <GL/glut.h>
#include <gtest/gtest.h>
#include <string>
#include <unistd.h>
#include <errno.h>

void frameCount();
void display();
void keyboard();
void reshape();
extern bool g_suppressGraphics;
extern bool g_parallel;

bool glutInitialized=false;

namespace {
  class SceneTest : public ::testing::Test {
  protected:
    SceneTest() {
      g_suppressGraphics=false;
      g_parallel=false;

      if(!glutInitialized) {
	char *myargv [1];
	int myargc=1;
	myargv [0]=strdup ("beautify");
	glutInit(&myargc, myargv);
	glutInitialized=true;
      }
      
      scene = new Scene();
    }

    ~SceneTest() {
      //delete scene;
    }

    const char * workingDir="scenes";
    std::string filename="largescene1.dat";
    const char * tmpDir="/tmp";
    const char * outFilename="SceneTest.jpg";
    Scene * scene;

    void makeGradient() {
      int width=scene->getWindowWidth();
      int height=scene->getWindowHeight();
      for(int x=-10; x<(width+10); x++) {
	for(int y=-10; y<(height+10); y++) {
	  scene->putPixel(x,y,0.0,(1.0/width)*x,(1.0/height)*y);
	}
      }
    }
    
  };
}

TEST_F(SceneTest,defaultConstructor) {
}

TEST_F(SceneTest,ReadFile) {
  EXPECT_EQ(chdir(workingDir),0);
  scene->ReadFile(filename);
  EXPECT_EQ(chdir("../"),0);
}

TEST_F(SceneTest,WriteImage) {
  EXPECT_EQ(chdir(workingDir),0);
  scene->ReadFile(filename);
  EXPECT_EQ(chdir(tmpDir),0);
  EXPECT_EQ(unlink(outFilename),-1);
  scene->writeImage(outFilename);
  EXPECT_EQ(unlink(outFilename),0);
}

TEST_F(SceneTest,putPixel) {
  makeGradient();
}

TEST_F(SceneTest,repaint) {
  makeGradient();
  scene->repaint();
}

TEST_F(SceneTest,smoothLogicalImage) {
  makeGradient();
  scene->smoothLogicalImage();
}

TEST_F(SceneTest,setWindowSize) {
  for(int x=50; x<1000; x+=50) {
    scene->setWindowSize(x,x);
    makeGradient();
    EXPECT_EQ(scene->getWindowWidth(),x);
    EXPECT_EQ(scene->getWindowHeight(),x);
  }
}

TEST_F(SceneTest,draw) {
  scene->draw();
}

TEST_F(SceneTest,getLights) {
  auto result = scene->getLights();
  EXPECT_FALSE(result==NULL);
}

TEST_F(SceneTest,getSurfaces) {
  auto result = scene->getSurfaces();
  EXPECT_FALSE(result==NULL);
}

TEST_F(SceneTest,getWindowWidth) {
  scene->setWindowSize(100,50);
  EXPECT_EQ(100,scene->getWindowWidth());
}

TEST_F(SceneTest,getWindowHeight) {
  scene->setWindowSize(100,50);
  EXPECT_EQ(50,scene->getWindowHeight());
}

TEST_F(SceneTest,normalizeChannels) {
  scene->setWindowSize(200,200);
  scene->putPixel(10,10,1.0,0.0,0.0);
  scene->normalizeChannels();
}

//TEST_F(SceneTest,generateFiles) {
//}

TEST_F(SceneTest,setNumNeighbors) {
  scene->setNumNeighbors(15);
}
