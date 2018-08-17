// This may look like C code, but it is really -*- C++ -*-

// Scene.h

// (C) 2002 Anonymous1
// Modified 2002 Tom White

#ifndef SCENE_H
#define SCENE_H

#include "config.h"
#include <parallel_pm.h>
#include <fstream>

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

  // Scene Procedures

  // Read contents of a scene file, skipping over unimplemented features
  void ReadFile(std::string fileName);

  //renders a single frame into logicalImage
  void drawSingleFrame(double time);

  //Outputs image to filename specified
  void renderDrawnSceneToFile(const char * fileName);
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

  std::vector<Light *> * getLights();
  std::vector<Surface *> * getSurfaces();
  Camera * getCamera();
  int getWindowWidth();
  int getWindowHeight();

  void generateFiles(const char * filename, 
		     int startFrame,
		     int numFrames
		     );
  
  //renderer keeps its own copy; be sure to update
  //if one renderer is handling multiple scenes.  (Ha!)
  Point3Dd ambient;
  
  Renderer * myRenderer;
  int width;
  int height;

  bool hasImage; 

  double dtdf; //change in time per frame

  //call from glut's display() callback
  void glutDisplayCallback();
  
  //update time for scene and everything it owns that has knowledge of time.
  void setTime(double time);
  
  //sets Number of neighbors to use for luminance information
  void setNumNeighbors();
  void setNumNeighbors(int numNeighbors);

  void willHaveThisManyPhotonsThrownAtIt(int nPhotons);
  void willNotUseMoreThanThisManyPhotonsPerPixel(int neighbors);
  void willNeverDiscardPhotonsThisClose(int minDist);
  void willAlwaysDiscardPhotonsThisFar(int maxDist);
  
  
protected:

  int numNeighbors;
  int nPhotons;
  int minDist, maxDist;
  double currentTime;

  //generates a logicalImage from the Scene and returns a pointer to it.
  // note this is the same as this->logicalImage, and caller should NOT delete.
  double * toLogicalImage();
  
  //tells myRenderer to throw photons into the scene
  //void throwPhotonsIn();

  //sets photon map parameters to those we have saved
  void tuneMapParams();
  
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
  std::vector<Light *> * lights;

  //And a std::list of all the surfaces
  std::vector<Surface *> * surfaces;

  //And a std::list of all the materials
  std::vector<Material *> * materials;

  //And a std::list of all the camera available
  std::vector<Camera *> * cameras;
  int currentCamera;

  //And a list of all of the photon maps in use
  std::vector<PhotonMap *> * photonMaps;

  //helper functions.  We keep these around to provide an abstraction
  //layer in case we want to change implementation later.  Also, they
  //make code prettier.
  inline void addLight(Light *);
  inline void addSurface(Surface *);
  inline void addCamera(Camera *);
  inline void addMaterial(Material *);

  //Creates a shader of type shaderType
  Shader * createShader(int shaderType, 
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




