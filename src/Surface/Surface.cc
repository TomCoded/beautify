//Surface.cc
//(C)  Tom White
#include <allIncludes.h>

Surface::Surface():
  created(0)
{
  UNIMPLEMENTED("Surface.Surface()")
}

Surface::Surface(Surface& other):
  created(0)
{
  surShape = other.surShape;
  surShader = other.surShader;
  transLocalToWorld=other.transLocalToWorld;
  transWorldToLocal=other.transWorldToLocal;
  transLocalToWorldNormal=other.transLocalToWorldNormal;
  translateLocalToWorld=other.translateLocalToWorld;
  translateWorldToLocal=other.translateWorldToLocal;
}

Surface::Surface(Shape *shape, std::shared_ptr<Shader> shader,
		 Transform4Dd tLtW, 
		 Transform4Dd tWtL,
		 Transform4Dd trLtW,
		 Transform4Dd trWtL
		 ):
  created(0), 
  surShape(shape),
  transLocalToWorld(tLtW),
  transWorldToLocal(tWtL),
  translateLocalToWorld(trLtW),
  translateWorldToLocal(trWtL)
{
  this->surShader=shader;
  transLocalToWorldNormal=transWorldToLocal.transpose();
}

Surface::Surface(Shape *s, Material *m):
  created(0),
  surShape(s),
  surMat(m),
  timedependent(false)
{
  tLocalToWorldNormal=MakeTranslation(0,0,0);
  tLocalToWorld=MakeTranslation(0,0,0);
  tWorldToLocal=MakeTranslation(0,0,0);
}

Surface::Surface(Shape *s, std::shared_ptr<Shader> shader,
		 Material *m,
		 Transform4Dd localToWorld,
		 Transform4Dd worldToLocal,
		 Transform4Dd localToWorldNormal
		 ):
  created(0),
  surShape(s),
  surMat(m),
  tLocalToWorldNormal(localToWorldNormal),
  tWorldToLocal(worldToLocal),
  tLocalToWorld(localToWorld),
  timedependent(false)
{
  surShader=shader;
}

Surface::Surface(Shape *s, std::shared_ptr<Shader> shader,
		 Material *m,
		 FunTransform4Dd * ftLocalToWorld,
		 FunTransform4Dd * ftWorldToLocal,
		 FunTransform4Dd * ftLocalToWorldNormal
		 ):
  created(0),
  surShape(s),
  surMat(m),
  ftLocalToWorld(ftLocalToWorld),
  ftWorldToLocal(ftWorldToLocal),
  ftLocalToWorldNormal(ftLocalToWorldNormal),
  timedependent(true)
{
  setTime(0);
}

Surface::~Surface()
{
  if(created) 
    {
      delete surShape;
    }
}

void Surface::setTime(double t)
{
  if(timedependent)
    {
      this->t=t;
      ftLocalToWorldNormal->t=t;
      ftLocalToWorld->t=t;
      ftWorldToLocal->t=t;
      tLocalToWorld=ftLocalToWorld->getStaticTransform();
      tLocalToWorldNormal=ftLocalToWorldNormal->getStaticTransform();
      tWorldToLocal=ftWorldToLocal->getStaticTransform();
    }
}

bool Surface::participates() const
{
  return surMat->participates();
}

bool Surface::contains(Point3Dd &loc) const
{
  if(implicit(loc) < 0)
    {
      return true;
    }
  else
  {
    return false;
  }
}

double Surface::implicit(Point3Dd &loc) const
{
  //default to outside for now.
  Point4Dd loc4(loc.x, loc.y, loc.z, 1);
  loc4 = worldToLocal(loc4);
  Point3Dd loc3(loc4.x, loc4.y, loc4.z);
  return surShape->implicit(loc3);
}

Ray Surface::localToWorldNormal(Ray &r) const
{
  Point4Dd src4 = localToWorldNormal(r.src);
  Point4Dd dst4 = localToWorldNormal(r.dir);
  return Ray(src4.x,
	     src4.y,
	     src4.z,
	     dst4.x,
	     dst4.y,
	     dst4.z
	     );
}

Ray Surface::localToWorld(Ray &r) const
{
  Point4Dd src4 = localToWorld(r.src);
  Point4Dd dst4 = localToWorld(r.dir);
  return Ray(src4.x,
	     src4.y,
	     src4.z,
	     dst4.x,
	     dst4.y,
	     dst4.z
	     );
}

Ray Surface::worldToLocal(Ray &r) const
{
  Point4Dd src4 = worldToLocal(r.src);
  Point4Dd dst4 = worldToLocal(r.dir);
  return Ray(src4.x,
	     src4.y,
	     src4.z,
	     dst4.x,
	     dst4.y,
	     dst4.z
	     );
}

Point4Dd Surface::localToWorldNormal(Point4Dd &p) const
{
  return tLocalToWorldNormal*p;
}

Point3Dd Surface::localToWorldNormal(Point3Dd &p) const
{
  Point4Dd scratch(p,0);
  scratch = localToWorldNormal(scratch);
  return Point3Dd(scratch.x,
		  scratch.y,
		  scratch.z
		  );
}

Point4Dd Surface::localToWorld(Point4Dd &p) const
{
  return tLocalToWorld*p;
}

Point4Dd Surface::worldToLocal(Point4Dd &p) const
{
  return tWorldToLocal*p;
}

double Surface::closestIntersect(Ray &r) const
{
  Ray localRay = worldToLocal(r);
  return surShape->closestIntersect(localRay);
}

double Surface::closestIntersect(double x,
				 double y,
				 double z,
				 double dx,
				 double dy,
				 double dz
				 ) const
{
  Ray worldRay(x,y,z,dx,dy,dz);
  Ray localRay = worldToLocal(worldRay);
  return surShape->closestIntersect(localRay);  
}

//returns point of closest intersect in world coords.
Point3Dd Surface::pointOfClosestIntersect(
					  double x,
					  double y,
					  double z,
					  double dx,
					  double dy,
					  double dz
					  ) const
{
  Ray worldRay(x,y,z,dx,dy,dz);
  Ray localRay = worldToLocal(worldRay);
  double t = surShape->closestIntersect(localRay);  
  Point4Dd rv = worldRay.GetPointAt(t);
  return Point3Dd(rv.x,rv.y,rv.z);
}

//returns normal at closestIntersect(x,y,z,dx,dy,dz)
Point3Dd Surface::getNormalAt(double x,
			      double y,
			      double z,
			      double dx,
			      double dy,
			      double dz
			    ) const
{
  Ray r(x,y,z,dx,dy,dz);
  return getNormalAt(r);
}

//returns normal at closestIntersect(r)  
// r in world coordinates
Point3Dd Surface::getNormalAt(Ray &r) const
{
  Ray localRay = worldToLocal(r);
  Ray rv = surShape->getNormal(localRay);
  Point4Dd rv4 = localToWorldNormal(rv.dir); 
#if 0
  std::cout << "Normal in Shape Coords: " << rv.dir <<std::endl;
  std::cout << "Normal in World Coords: " << rv4 <<std::endl;
  std::cout << "Matrix was " << tLocalToWorldNormal <<std::endl;
  std::cout << "---------------------------------------\n";
#endif
  return Point3Dd(rv4.x,rv4.y,rv4.z);
}

int Surface::nDoRoulette(Photon &p) const
{
  return surMat->nRoulette(p);
}

int Surface::nDoPartRoulette(Photon &p) const
{
  return ((Participating*)surMat)->nRoulette(p);
}

void Surface::DoVolBRDF(Photon &p) const
{
  //don't adjust to local coords b/c doesn't matter
  //for simple media
  return ((Participating*)surMat)->DoBRDF(p);
}

void Surface::DoBRDF(Photon &p)
{
  Photon local_p=p;
  Point4Dd wc(local_p.dx, local_p.dy, local_p.dz,1);
  wc=worldToLocal(wc);
  local_p.dx = wc.x;
  local_p.dy = wc.y;
  local_p.dz = wc.z;
  return surMat->DoBRDF(local_p);
}

Point3Dd Surface::DoSpecBRDF(Point3Dd& incident, Point3Dd& reflected)
{
  Point4Dd inc(incident,1);
  Point4Dd ref(reflected,1);
  inc=worldToLocal(inc);
  ref=worldToLocal(ref);
  Point3Dd inc2(inc.x,inc.y,inc.z);
  Point3Dd ref2(ref.x,ref.y,ref.z);
  return surMat->DoSpecBRDF(inc2,ref2);
}

double Surface::getSpecCo(int nLightingModel)
{
  return surMat->getSpecCo(nLightingModel);
}





