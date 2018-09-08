// -*- c++ -*-
#ifndef RENDERER_H_
#define RENDERER_H_

//Renderer.h
//(C) Tom White

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

  //destructor:
  ~Renderer();

  //Management functions

  //runs the photon mapper.
  PhotonMap * map();
  PhotonMap * map(int nPhotons);

  //traces a photon map
  //throws rays from the eye toward the scene
  //and calls back Scene::putPixel() with
  //color information for each pixel
  void showMap(PhotonMap *);
  void showMap(PhotonMap *, int start, int pixels);
    
  //functions for shader

  //Lighting functions
  //returns a std::vector of all apparent light sources in the scene.
  //these are new light sources! remember to delete!
  std::shared_ptr<std::vector<std::shared_ptr<Light>>> getApparentLights(Point3Dd);

  //returns a std::vector of light sources in the scene
  std::shared_ptr<std::vector<std::shared_ptr<Light>>> getAllLights();

  //returns all the lights that are fully visible
  std::shared_ptr<std::vector<std::shared_ptr<Light>>> getVisibleLights(Point3Dd);

  //returns remaining surfaces behind the last intersected surface
  std::shared_ptr<std::vector<std::shared_ptr<Surface>>> getOtherSurfaces();
  
  //deallocates all the lights in a std::vector, then destroys it
  void deAllocate(std::shared_ptr<std::vector<std::shared_ptr<Light>>>);

  //I/O functions
  std::istream& in(std::istream&);
  std::ostream& out(std::ostream&);

  //ambient light of the scene
  Point3Dd ambient;

  Point3Dd getColor(Ray * sampleRay);
  Point3Dd getColor(Ray * sampleRay, 
		    std::shared_ptr<std::vector<std::shared_ptr<Surface>>> surfaces
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
  int photonsEmittedCount;
  
  void participantMarch(Photon &p, 
			std::shared_ptr<Surface> medium
			);
  Point3Dd estimateExtinctionCoefficient(std::shared_ptr<Surface> surface,
				     Participating *material,
				     Photon &p,
				     Point3Dd &location,
				     int extinctionSamplesPerStep=3
				     );
  
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
					  const std::shared_ptr<Surface> surface,
					  const int marchsize) const;

  //Returns the sum of the illuination at a point in a participating
  //medium due to single scatterring, invoking
  //getIlluminationAtPointInMedium to determine direct info and
  //ray-marching out the back of the segment.
  Point3Dd getIlluminationInMedium(const Point3Dd& point,
				   const Point3Dd& dir,
				   const std::shared_ptr<Surface> surface,
				   const int marchsize) const;

  std::shared_ptr<Surface> closestSurfaceAlongRay(Ray * sampleRay,
                                   double &tClose);

  static const int maxdepth=10;
  int recursionDepth;
  Scene * myScene;

  std::shared_ptr<Camera> currentCamera;

  void newMap();
  
  std::vector<std::shared_ptr<Surface>> otherSurfaces;
};

#endif

