//(C) 2002 Tom White
#ifndef SURFACE_H_
#define SURFACE_H_

class Shape;
class Shader;

#include <FunTransform4Dd.h>
#include <allIncludes.h>
#include <Material/Material.h>

class Surface
{
 public:
  Surface();
  Surface(Surface&);

  //ray-tracing constructor
  Surface(Shape *, Shader *,Transform4Dd tLtW,Transform4Dd tWtL,
	  Transform4Dd trLtW,
	  Transform4Dd trWtL
	  );

  //photon-mapping constructors
  //caller responsible for destroying these once they've been allocated.
  Surface(Shape *, Material *);
  // FunTransform4Dd localToWorld,
  // FunTransform4Dd worldToLocal);

  //Major photon-mapping constructors
  Surface(Shape *, 
	  Material *,
	  Transform4Dd localToWorld,
	  Transform4Dd worldToLocal,
	  Transform4Dd localToWorldNormal
	  );

  Surface(Shape *,
	  Material *,
	  FunTransform4Dd * ftLocalToWorld,
	  FunTransform4Dd * ftWorldToLocal,
	  FunTransform4Dd * ftLocalToWorldNormal
	  );

  //updates the state of the surface to reflect its position,
  //etc... at time t
  void setTime(double t);

  ~Surface();
  
  // Matrices for coordinate transformation.  Left public to save cost of
  // function wrapper.
  Transform4Dd transLocalToWorld;
  Transform4Dd transWorldToLocal;
  Transform4Dd transLocalToWorldNormal;
  
  //use is depredcated
  //need translations seperately to preserve ray direction
  Transform4Dd translateLocalToWorld;
  Transform4Dd translateWorldToLocal;
  
  //each shape should be updated to have a material associated
  //with it; the material should be by pointer,
  //and the creator should be responsible for destoying the materials
  //afterwords.
  Shape * surShape;

  //use is deprecated
  Shader * surShader;

  //returns true if surface is a participating medium
  bool participates() const;

  //true if loc is inside the surface volume
  bool contains(Point3Dd &loc) const;

  //returns the value of the implicit function of the surface
  //at location loc.
  double implicit(Point3Dd &loc) const;
  
  //Returns smallest non-negative t-value of intsersection of r with
  //the surface.  Takes a ray in *World* coordinates.
  double closestIntersect(Ray &r) const;
  double closestIntersect(double x,
			  double y,
			  double z,
			  double dx,
			  double dy,
			  double dz
			  ) const;

  Point3Dd pointOfClosestIntersect(
				   double x,
				   double y,
				   double z,
				   double dx,
				   double dy,
				   double dz
				   ) const;

  //returns normal at closestIntersect(r)
  // r is in world coordinates
  Point3Dd getNormalAt(Ray &r) const;
  Point3Dd getNormalAt(double x,
		       double y,
		       double z,
		       double dx,
		       double dy,
		       double dz
		       ) const;

  double tScratch; //a scratch value used by Renderer

  int nDoRoulette(Photon &p) const;
  //for participating media
  int nDoPartRoulette(Photon &p) const;
  
  void DoVolBRDF(Photon &p) const;
  void DoBRDF(Photon &p); //updates values of p with BRDF.
  //in the case of a lambertian surface, uses straight material
  //properties.  For diffuse component of BRDF.

  Point3Dd DoSpecBRDF(Point3Dd& incident, Point3Dd& reflected);
  //returns computed BRDF for given incident & reflected specular light.

  //returns specular coefficient of the surface
  double getSpecCo(int nLightingModel);

 protected:

  bool timedependent;

  double t; //the time of the event
  FunTransform4Dd * ftLocalToWorld;
  FunTransform4Dd * ftWorldToLocal;
  FunTransform4Dd * ftLocalToWorldNormal;

  //Transform4Dd's that are current copies of the functional ones.
  Transform4Dd tLocalToWorld;
  Transform4Dd tWorldToLocal;
  Transform4Dd tLocalToWorldNormal;

  Ray localToWorldNormal(Ray&) const;
  Ray localToWorld(Ray&) const;
  Ray worldToLocal(Ray&) const;

  Point4Dd localToWorldNormal(Point4Dd&) const;
  Point3Dd localToWorldNormal(Point3Dd&) const;
  Point4Dd localToWorld(Point4Dd&) const;
  Point4Dd worldToLocal(Point4Dd&) const;

  Material * surMat;

  int created; //if Shape* and Shader* were created by this object,
  //and should be destroyed by it when it exits.
};

#endif
