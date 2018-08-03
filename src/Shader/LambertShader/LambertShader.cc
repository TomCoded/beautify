//LambertShader.cc
//(C) 2002 Tom White
#include <allIncludes.h>

//default constructor
LambertShader::LambertShader():
  ambient(0,0,0), diffuse(0,0,0)
{
  transparency=0;
}

LambertShader::LambertShader(Point3Dd amb,
			     Point3Dd diff,
			     double transparency,
			     Renderer * sceneRend):
  ambient(amb),
  diffuse(diff)
{ //can't pre-body initialize b/c is from parent class
  sceneRenderer=sceneRend; 
  this->transparency=transparency;
}

LambertShader::LambertShader(LambertShader& other)
{
  if(this!=&other)
    {
      ambient=other.ambient;
      diffuse=other.diffuse;
      sceneRenderer=other.sceneRenderer;
      transparency=other.transparency;
    }
}

//create method for Shape classes
LambertShader * LambertShader::create(Renderer * sceneRenderer)
{
  ambient = Point3Dd(0,0,0);
  diffuse = Point3Dd(0,0,0);
  transparency = 0;
  this->sceneRenderer = sceneRenderer;
}

CLONEMETHOD(LambertShader)

LambertShader::~LambertShader() {}

Point3Dd LambertShader::getAmbientColor()
{
  return Point3Dd(sceneRenderer->ambient.x * ambient.x,
		  sceneRenderer->ambient.y * ambient.y,
		  sceneRenderer->ambient.z * ambient.z
		  );
}

Point3Dd LambertShader::getDiffuseColor(Light ** itVisibleLights, 
					Hit &hitPoint) 
{
  double diffuseMult, temp, r, g, b;
  r=g=b=0;
  diffuseMult =
    ((*itVisibleLights)->getRayTo(hitPoint.point).dir).dot(hitPoint.normal.dir);
  
  //red
  temp=(*itVisibleLights)->diffuse.x*diffuse.x*diffuseMult;
#ifdef DIFFUSE //if there's a global diffuse modifier
  temp=temp*DIFFUSE;
#endif
  if(temp>0) r+=temp;
  //green
  temp=(*itVisibleLights)->diffuse.y*diffuse.y*diffuseMult;
#ifdef DIFFUSE
  temp=temp*DIFFUSE;
#endif
  if(temp>0) g+=temp;
  //blue
  temp=(*itVisibleLights)->diffuse.z*diffuse.z*diffuseMult;
#ifdef DIFFUSE
  temp=temp*DIFFUSE;
#endif
  if(temp>0) b+=temp;
  return Point3Dd(r,g,b);
}

Point3Dd LambertShader::getColor(Hit &hitPoint)
{
  return getLambertColor(hitPoint);
}

Point3Dd LambertShader::getLambertColor(Hit &hitPoint) 
{
  vector<Light *> * Lights;
  vector<Light *>::iterator itLights;

  //Get a vector of all visible Lights in the Scene.
  //The renderer adjusted them for transparency
  //Lights = sceneRenderer->getApparentLights(hitPoint.point); 
  //Get a vector of all lights in the scene,
  //since that's the project description
  Lights = sceneRenderer->getAllLights();
  itLights = Lights->begin();

  //set ambient lighting
  Point3Dd theLight = getAmbientColor();

  //for each Light, add the diffuse components
  while(itLights != Lights->end() )
    { 
      theLight+=getDiffuseColor(itLights++,hitPoint);
    }
  return theLight;
}

istream& LambertShader::in(std::istream&)
{
}

ostream& LambertShader::out(std::ostream&)
{
}
