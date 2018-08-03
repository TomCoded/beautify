#include <allIncludes.h>

//default constructor
PhongSRTShader::PhongSRTShader():
  specular(0,0,0),
  specularCoefficient(1),
  reflection(0)
  //  :ambient(0,0,0), diffuse(0,0,0)
{
  ambient=Point3Dd(0,0,0);
  diffuse=Point3Dd(0,0,0);
  transparency=0;
}

PhongSRTShader::PhongSRTShader(Point3Dd amb,
			       Point3Dd diff,
			       Point3Dd spec,
			       double specCo,
			       double trans,
			       double reflection,
			       Renderer * sceneRend):
  specular(spec),
  specularCoefficient(specCo),
  reflection(reflection)
{ //can't pre-body initialize b/c is from parent class
  ambient=amb;
  diffuse=diff;
  sceneRenderer=sceneRend;
  transparency=trans;
}

PhongSRTShader::PhongSRTShader(PhongSRTShader& other)
{
  if(this!=&other)
    {
      ambient=other.ambient;
      diffuse=other.diffuse;
      sceneRenderer=other.sceneRenderer;
      specularCoefficient=other.specularCoefficient;
      transparency=other.transparency;
      reflection=other.reflection;
    }
}

//create method for Shape classes
PhongSRTShader * PhongSRTShader::create(Renderer * sceneRenderer)
{
  ambient = Point3Dd(0,0,0);
  diffuse = Point3Dd(0,0,0);
  this->sceneRenderer = sceneRenderer;
  specularCoefficient = 0;
  reflection=0;
}

CLONEMETHOD(PhongSRTShader)

PhongSRTShader::~PhongSRTShader() {}

Point3Dd PhongSRTShader::getSpecularColor(Light ** itVisibleLights, Hit &hitPoint)
{
  double cosf;
  double r,g,b;
#define DirToLight ((*itVisibleLights)->getRayTo(hitPoint.point).dir)
#define DirOfNormal hitPoint.normal.dir
#define DoubleLightDotNormal (2*((DirToLight.dot(DirOfNormal))))
#define ReflectionVectorPlusDirOfLight \
    (DirOfNormal*(DoubleLightDotNormal/((DirOfNormal.norm()*DirOfNormal.norm()))))
  //get the reflection vector
  Point3Dd reflection(((DirToLight)*-1)+ReflectionVectorPlusDirOfLight);
  //get cosine between reflection vector and sample ray
  cosf =
    ((reflection.dot(hitPoint.sampleRay.dir*-1))/(reflection.norm()*(hitPoint.sampleRay.dir.norm())));
#undef DirToLight
#undef DirOfNormal
#undef DoubleLightDotNormal
#undef ReflectionVectorPlusDirOfLight
  if(cosf>0)
    {
      cosf = powf(cosf,specularCoefficient);
      r = (*itVisibleLights)->specular.x * specular.x * cosf;
#ifdef SPECULAR
      r = r * SPECULAR;
#endif
      g = (*itVisibleLights)->specular.y * specular.y * cosf;
#ifdef SPECULAR
      g = g * SPECULAR;
#endif
      b = (*itVisibleLights)->specular.z * specular.z * cosf;
#ifdef SPECULAR
      b = b * SPECULAR;
#endif
    }
  else
      r=g=b=0;
  if(r<0) r=0;
  else if(r>1) r=1;
  if(g<0) g=0;
  else if(g>1) g=1;
  if(b<0) b=0;
  else if(b>1) b=1;
  return Point3Dd(r,g,b);
}

Point3Dd PhongSRTShader::getColor(Hit &hitPoint)
{
  static int depth=0;
  static Point3Dd lastHitPoint=Point3Dd(0,0,0);
  vector<Light *> * visibleLights;
  vector<Light *>::iterator itVisibleLights;

  //Get a vector of all Lights in the Scene that are at all visible.
  visibleLights = sceneRenderer->getApparentLights(hitPoint.point); 
  itVisibleLights = visibleLights->begin();

  //set ambient lighting
  Point3Dd theLight = getAmbientColor();

  //for each Light, add the diffuse components
  while(itVisibleLights != visibleLights->end() )
    { //get diffuse color from parent class LambertShader
      theLight+=getDiffuseColor(itVisibleLights,hitPoint);
      theLight= theLight+getSpecularColor(itVisibleLights++,hitPoint);
    }
  //now scale for transparency
  theLight = theLight*(1-transparency);
  //now add objects directly behind us along the ray.
  //note that we start off directly behind us.
  //first we get the tvalue of our other surface.

  if(depth <= MAXREFLECTIONDEPTH)
    {
#define DirToLight (hitPoint.sampleRay.dir*-1)
#define DirOfNormal hitPoint.normal.dir
#define DoubleLightDotNormal (2*((DirToLight.dot(DirOfNormal))))
#define ReflectionVectorPlusDirOfLight \
    (DirOfNormal*(DoubleLightDotNormal/((DirOfNormal.norm()*DirOfNormal.norm()))))
      Ray reflectedRay(hitPoint.point,
		       Point3Dd(((DirToLight*-1)+ReflectionVectorPlusDirOfLight))
		       );
      depth++;
      theLight+=sceneRenderer->getColor(&reflectedRay)*(1-reflection);
      depth--;
    }
#undef DirToLight
#undef DirOfNormal
#undef DoubleLightDotNormal
#undef ReflectionVectorPlusDirOfLight

  sceneRenderer->deAllocate(visibleLights);
  return theLight;
}

std::istream& PhongSRTShader::in(std::istream&)
{
}

std::ostream& PhongSRTShader::out(std::ostream&)
{
}
