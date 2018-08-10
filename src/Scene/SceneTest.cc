#include "Scene.h"
#include <GL/glut.h>
#include <gtest/gtest.h>
#include <string>
#include <unistd.h>

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
    Scene * scene;
    
  };
}

TEST_F(SceneTest,defaultConstructor) {
}

TEST_F(SceneTest,ReadFile) {
  EXPECT_EQ(chdir(workingDir),0);
  scene->ReadFile(filename);
}

TEST_F(SceneTest,WriteImage) {
}

TEST_F(SceneTest,putPixel) {
}

TEST_F(SceneTest,repaint) {
}

TEST_F(SceneTest,smoothLogicalImage) {
}

TEST_F(SceneTest,setWindowSize) {
}

TEST_F(SceneTest,draw) {
}

TEST_F(SceneTest,getLights) {
}

TEST_F(SceneTest,getSurfaces) {
}

TEST_F(SceneTest,getWindowWidth) {
}

TEST_F(SceneTest,getWindowHeight) {
}

TEST_F(SceneTest,normalizeChannels) {
}

TEST_F(SceneTest,generateFiles) {
}

TEST_F(SceneTest,setNumNeighbors) {
}
