// -*- c++ -*-
#ifndef RENDERER_H_
#define RENDERER_H_

//Renderer.h
//(C) 2002 Tom White

class Camera;

#include <allIncludes.h>
#include <Photon/Photon.h>
#include <Point3Dd.h>
#include <Scene/Scene.h>
#include <PhotonMap/PhotonMap.h>
#include <Material/Participating/Participating.h>

class Renderer
{
 public:
  //constructors:
  Renderer();
  Renderer(Renderer&);
  Renderer(Scene * myScene);

  void setScene(Scene * myScene);
  //sets random number seed for light sources
  void setSeed(int s);

  //destructor:
  ~Renderer();

  //Management functions
  //runs the ray tracer.
  int run();

  //runs the photon mapper.
  PhotonMap * map();
  PhotonMap * map(int nPhotons);

  //traces a photon map
  void showMap(PhotonMap *);
  void showMap(PhotonMap *, int start, int pixels);
    
  //functions for shader

  //Lighting functions
  //returns a vector of all apparent light sources in the scene.
  //these are new light sources! remember to delete!
  vector<Light *> * getApparentLights(Point3Dd);

  //returns a vector of light sources in the scene
  vector<Light *> * getAllLights();

  //returns all the lights that are fully visible
  vector<Light *> * getVisibleLights(Point3Dd);

  //returns remaining surfaces behind the last intersected surface
  vector<Surface *> * getOtherSurfaces();
  
  //deallocates all the lights in a vector, then destroys it
  void deAllocate(vector <Light *> *);

  //I/O functions
  istream& in(istream&);
  ostream& out(ostream&);

  //ambient light of the scene
  Point3Dd ambient;

  Point3Dd getColor(Ray * sampleRay);
  Point3Dd getColor(Ray * sampleRay, 
		    vector<Surface *> * surfaces
		    );

  //Traces the course of a photon and returns its final location
  Photon& tracePhoton(Photon &p, int recurse=0);

  //resets the incident direction of a photon after collision.
  //keeping this in a seperate function let's us preserve ability
  //to switch between halfway & phong models easily.
  inline Photon & resetIncidentDir(Photon &p,
				   Point3Dd & normal
				   );
  
  PhotonMap * getVolMap();

 protected:

  PhotonMap * pMap;
  PhotonMap * pVolMap;

  void participantMarch(Photon &p, 
			Surface * medium,
			int single_scatter=0
			);

  int seed;

  //returns the specular component of the light
  //halfway method
  Point3Dd getSpecularColor(Ray * sampleRay);

  //returns diffuse component of light
  Point3Dd mapGetColor(Ray * sampleRay,
		       PhotonMap * map
		       );

  //Returns the first component of the formula to calculate direct
  //illuination for a point in a participating medium.  This is the
  //contribution from the light sources directly, attenuated for the
  //distance in the medium that the light has already travelled.
  Point3Dd getIlluminationAtPointInMedium(const Point3Dd &point,
					  const Point3Dd &dir,
					  const Surface * surface,
					  const int marchsize) const;

  //Returns the sum of the illuination at a point in a participating
  //medium due to single scatterring, invoking
  //getIlluminationAtPointInMedium to determine direct info and
  //ray-marching out the back of the segment.
  Point3Dd getIlluminationInMedium(const Point3Dd& point,
				   const Point3Dd& dir,
				   const Surface * surface,
				   const int marchsize) const;
  

  static const int maxdepth=10;
  int recursionDepth;
  Scene * myScene;

  Camera * currentCamera;
protected:
  vector<Surface *> otherSurfaces;
};

#endif

