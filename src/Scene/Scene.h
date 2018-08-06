// This may look like C code, but it is really -*- C++ -*-

// Scene.h

// (C) 2002 Anonymous1
// Modified 2002 Tom White

#ifndef SCENE_H
#define SCENE_H

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
  void ReadFile(string fileName);

  //Outputs image to filename specified
  void writeImage(const char * fileName);

  //writes a pixel to a graphics window.  If x>width or y>height,
  //resizes window to size x,y
  void putPixel(int x, int y, Point3Dd color);
  void putPixel(int x, int y, double r, double g, double b);

  //repaints scene from a stored logical image
  void repaint();
  //smooths an image after it's been rendered.
  void smoothLogicalImage();

  void setWindowSize(int x, int y);

  //tells myRenderer to draw the scene
  void draw();

  //Scene Functions
  vector<Light *> * getLights();
  vector<Surface *> * getSurfaces();
  Camera * getCamera();
  int getWindowWidth();
  int getWindowHeight();
  //normalizes each color channel.
  void normalizeChannels();

  void generateFiles(const char * filename, 
		     int startFrame,
		     int numFrames, 
		     int photons,
		     int neighbors,
		     double minDist,
		     double maxDist
		     );
  
  //renderer keeps its own copy; be sure to update
  //if one renderer is handling multiple scenes.  (Ha!)
  Point3Dd ambient;
  
  Renderer * myRenderer;
  int width;
  int height;

  bool hasImage; 

  double dtdf; //change in time per frame

  //sets Number of neighbors to use for luminance information
  void setNumNeighbors();
  void setNumNeighbors(int numNeighbors);
  
protected:

  int numNeighbors;
  
#ifdef PARALLEL
  //parallel stuff
  //procedures
  void drawParallel();

  //variables
  double * localLogical;
  int localLogicalStart;
  int localLogicalSize;
  bool doingLocalPart;

#endif
  bool map_too_small; 

  Material * lastMaterial; //for readscene recursion
  //true if we're repainting a scene;
  //false if we're not.
  bool paintingFromLogical;

  //Our scene should have information about the graphics window being
  //used.

  bool logicalRender; //true if we're not acutally rendering to a window.

  //This is a logical image of the picture we're rendering, in RGB
  //format
  double * logicalImage;

  //  Renderer * myRenderer;

  //We also want information about the lights in a scene
  vector<Light *> * lights;

  //And a list of all the surfaces
  vector<Surface *> * surfaces;

  //And a list of all the materials
  vector<Material *> * materials;

  //And finally, a list of all the camera available
  vector<Camera *> * cameras;
  int currentCamera;

  //helper functions.  We keep these around to provide an abstraction
  //layer in case we want to change implementation later.  Also, they
  //make code prettier.
  inline void addLight(Light *);
  inline void addSurface(Surface *);
  inline void addCamera(Camera *);
  inline void addMaterial(Material *);

  //updates all objects in the scene to reflect current time
  void setTime(double t);

  //Creates a shader of type shaderType
  Shader * createShader(int shaderType, 
			Point3Dd matAmbient,
			Point3Dd matDiffuse,
			Point3Dd matSpecular,
			double matSpecExp,
			double matReflection,
			double matTransparent
			);
  
private:


  // skip the parameters of any non-implemented scene file feature
  // assumes that parameter lists are either single strings w/o white space
  // or properly parenthetically nested
  ifstream& skipDescription(ifstream& ifs);
};

#endif SCENE_H




