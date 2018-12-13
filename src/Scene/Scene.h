// This may look like C code, but it is really -*- C++ -*-

// Scene.h

// Modified  Tom White

#ifndef SCENE_H
#define SCENE_H

#define MAX_FILENAME_LEN 256

#include "config.h"
#include <parallel_pm.h>
#include <fstream>
#include <memory>

class Scene;
class Light;
class Renderer;

#include <allIncludes.h>
#include <Material/Material.h>
#include <Surface/Surface.h>
#include <vector>
#include <Camera/Camera.h>
#include <Renderer/Renderer.h>

////////////////////
// class Scene //
////////////////////

// A scene container 
//

class Scene {

public:

  // essentials

  // default constructor
  Scene();

  // destructor
  ~Scene();

  //initializes MPI
  static int initMPI();
  
  // Read contents of a scene file, skipping over unimplemented features
  void ReadFile(std::string fileName);
  
  //renders a single frame into logicalImage
  void drawSingleFrame(double time);

  //Outputs image to filename specified
  void renderDrawnSceneToFile();
  void renderDrawnSceneToWindow();
  

  //writes a pixel to a graphics window.
  void putPixel(int x, int y, Point3Dd color);
  void putPixel(int x, int y, double r, double g, double b);

  //repaints scene from a stored logical image
  void repaint();
  //smooths an image after it's been rendered.
  void smoothLogicalImage();
  //normalizes each color channel.
  void normalizeChannels();

  void setWindowSize(int x, int y);

  std::shared_ptr<std::vector<std::shared_ptr<Light>>> getLights();
  std::shared_ptr<std::vector<std::shared_ptr<Surface>>> getSurfaces();
  std::shared_ptr<Camera> getCamera();
  int getWindowWidth();
  int getWindowHeight();

  //renderer keeps its own copy; be sure to update
  //if one renderer is handling multiple scenes.  (Ha!)
  Point3Dd ambient;
  
  Renderer * myRenderer;
  int width;
  int height;

  bool hasImage; 

  double dtdf; //change in time per frame

  //starts scene and glut loops
  void startMainLoop(int rank);
  
  //call from glut's display() callback
  void glutDisplayCallback();
  
  //update time for scene and everything it owns that has knowledge of time.
  void setTime(double time);
  
  //sets Number of neighbors to use for luminance information
  void setNumNeighbors();
  void setNumNeighbors(int numNeighbors);

  void willWriteFilesWithBasename(const char *filename);
  void willWriteThisManyFrames(int willWriteThisManyFrames);
  void willStartOnFrame(int willStartOnFrame);
  
  void willHaveThisManyPhotonsThrownAtIt(int nPhotons);
  void willNotUseMoreThanThisManyPhotonsPerPixel(int neighbors);
  void willNeverDiscardPhotonsThisClose(int minDist);
  void willAlwaysDiscardPhotonsThisFar(int maxDist);
  
  void closeChildren();
  
protected:

  
  int numNeighbors;
  int nPhotons;
  int minDist, maxDist;

  //start at 0
  double currentTime;
  int frame;
  int startsOnFrame;
  int writesThisManyFrames;

  //for program metrics
  double mapCreationTime;
  double treeCreationTime;
  
  //filename information for current frame
  char szFile[MAX_FILENAME_LEN];
  char szTail[16];
  int nBaseLen;
  void setFilename();

  //runs the Scene.
  void mainLoop();
  
  //generates a logicalImage from the Scene and returns a pointer to it.
  // note this is the same as this->logicalImage, and caller should NOT delete.
  double * toLogicalImage();
  
  //tells myRenderer to throw photons into the scene
  //void throwPhotonsIn();

  //sets photon map parameters to those we have saved
  void tuneMapParams();
  
  //process rank
  int rank; 
  
#ifdef PARALLEL
  //parallel stuff

  //called by regular toLogicalImage
  void toLogicalImageInParallel();

  //variables
  double * localLogical;
  int localLogicalStart;
  int localLogicalSize;
  bool doingLocalPart;

  void renderParallelFrame(int photons,
			   int nFrame,
			   int startFrame,
			   int neighbors,
			   double minDist,
			   double maxDist,
			   int rank,
			   int nodes
			   );
  
#endif

  bool map_too_small; 

  //for readscene recursion
  Material * lastMaterial; 

  //true if we're repainting a scene;
  //false if we're not.
  bool paintingFromLogical;

  //Our scene should have information about the graphics window being
  //used.

  //This is a logical image of the picture we're rendering, in RGB
  //format
  double * logicalImage;

  //  Renderer * myRenderer;

  //We also want information about the lights in a scene
  std::shared_ptr<std::vector<std::shared_ptr<Light>>> lights;

  //And a std::list of all the surfaces
  std::shared_ptr<std::vector<std::shared_ptr<Surface>>> surfaces;

  //And a std::list of all the materials
  std::vector<Material *> * materials;

  //And a std::list of all the camera available
  std::shared_ptr<std::vector<std::shared_ptr<Camera>>> cameras;
  int currentCamera;

  //And a list of all of the photon maps in use
  std::vector<PhotonMap *> * photonMaps;

  //helper functions.  We keep these around to provide an abstraction
  //layer in case we want to change implementation later.  Also, they
  //make code prettier.
  inline void addLight(std::shared_ptr<Light>);
  inline void addSurface(std::shared_ptr<Surface>);
  inline void addCamera(std::shared_ptr<Camera>);
  inline void addMaterial(Material *);

  //Creates a shader of type shaderType
  std::shared_ptr<Shader> createShader(int shaderType, 
			Point3Dd matAmbient,
			Point3Dd matDiffuse,
			Point3Dd matSpecular,
			double matSpecExp,
			double matReflection,
			double matTransparent
			);

  bool frames_are_being_rendered_to_files;

private:


  // skip the parameters of any non-implemented scene file feature
  // assumes that parameter std::lists are either single strings w/o white space
  // or properly parenthetically nested
  std::ifstream& skipDescription(std::ifstream& ifs);
};

#endif SCENE_H




