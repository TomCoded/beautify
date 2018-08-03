//PhongSShader.cc
//(C) 2002 Tom White
#include <allIncludes.h>

//default constructor
PhongSShader::PhongSShader():
  specular(0,0,0),
  specularCoefficient(1)
  //  :ambient(0,0,0), diffuse(0,0,0)
{
  ambient=Point3Dd(0,0,0);
  diffuse=Point3Dd(0,0,0);
  transparency=0;
}

PhongSShader::PhongSShader(Point3Dd amb,
			 Point3Dd diff,
			 Point3Dd spec,
			 double specCo,
			 double trans,
			 Renderer * sceneRend):
  specular(spec),
  specularCoefficient(specCo)
{ //can't pre-body initialize b/c is from parent class
  ambient=amb;
  diffuse=diff;
  sceneRenderer=sceneRend;
  transparency=trans;
}

PhongSShader::PhongSShader(PhongSShader& other)
{
  if(this!=&other)
    {
      ambient=other.ambient;
      diffuse=other.diffuse;
      sceneRenderer=other.sceneRenderer;
      specularCoefficient=other.specularCoefficient;
      transparency=other.transparency;
    }
}

//create method for Shape classes
PhongSShader * PhongSShader::create(Renderer * sceneRenderer)
{
  ambient = Point3Dd(0,0,0);
  diffuse = Point3Dd(0,0,0);
  this->sceneRenderer = sceneRenderer;
  specularCoefficient = 0;
}

CLONEMETHOD(PhongSShader)

PhongSShader::~PhongSShader() {}

Point3Dd PhongSShader::getSpecularColor(Light ** itVisibleLights, Hit &hitPoint)
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

Point3Dd PhongSShader::getColor(Hit &hitPoint)
{
  vector<Light *> * visibleLights;
  vector<Light *>::iterator itVisibleLights;

  //Get a vector of all Lights in the Scene that are visible.
  visibleLights = sceneRenderer->getVisibleLights(hitPoint.point); 
  itVisibleLights = visibleLights->begin();

  //set ambient lighting
  Point3Dd theLight = getAmbientColor();

  //for each Light, add the diffuse components
  while(itVisibleLights != visibleLights->end() )
    { //get diffuse color from parent class LambertShader
      theLight+=getDiffuseColor(itVisibleLights,hitPoint);
      theLight= theLight+getSpecularColor(itVisibleLights++,hitPoint);
    }
  return theLight;
}

istream& PhongSShader::in(istream&)
{
}

ostream& PhongSShader::out(ostream&)
{
}
