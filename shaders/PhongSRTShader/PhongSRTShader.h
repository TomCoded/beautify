//PhongSRTShader.h
//(C)  Tom White
#ifndef PHONGSRTSHADER_H_
#define PHONGSRTSHADER_H_

class Shader;
class LambertShader;

#include <allIncludes.h>

class PhongSRTShader: public LambertShader
{
 public:
  PhongSRTShader(); //default constructor
  //useful constructor
  PhongSRTShader(Point3Dd amb, 
		 Point3Dd diff,
		 Point3Dd spec,
		 double specCo,
		 double trans,
		 double reflection,
		 Renderer * sceneRend);
  PhongSRTShader(PhongSRTShader&); //copy constructor
  PhongSRTShader* create(Renderer *); //generic Shader create()
  PhongSRTShader* clone(); //generic Shader clone()

  ~PhongSRTShader();

  Point3Dd getColor(Hit&);

  std::istream& PhongSRTShader::in(std::istream&);
  std::ostream& PhongSRTShader::out(std::ostream&);

 protected:
  double specularCoefficient;
  double reflection;
  Point3Dd specular;
  Point3Dd getHalfwaySpecularColor(); //uses halfway speedup
  Point3Dd getSpecularColor(Light ** itVisibleLights, Hit &hitPoint); //uses the phong model
};

#endif















