//(C)  Tom White
#ifndef LAMBERTSHADER_H_
#define LAMBERTSHADER_H_

class Shader;
class Hit;
class Light;

#include <allIncludes.h>

class LambertShader: public Shader
{
 public:
  LambertShader(); //default constructor
  //useful constructor
  LambertShader(Point3Dd amb, Point3Dd diff, double transparency, Renderer * sceneRend);
  LambertShader(LambertShader&); //copy constructor
  LambertShader* create(Renderer *); //generic Shader create()
  LambertShader* clone(); //generic Shader clone()

  ~LambertShader();

  Point3Dd getColor(Hit&);

  std::istream& in(std::istream&);
  std::ostream& out(std::ostream&);

 protected:
  Point3Dd ambient;
  Point3Dd diffuse;
  Point3Dd getAmbientColor();
  Point3Dd getDiffuseColor(std::shared_ptr<Light> itVisibleLights, Hit&);
  //  Point3Dd getDiffuseColor(Light ** itVisibleLights, Hit&);
  Point3Dd getLambertColor(Hit&);
};

#endif















