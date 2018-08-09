//(C) 2002 Tom White
#ifndef PHONGSSHADER_H_
#define PHONGSSHADER_H_

class Shader;
class LambertShader;

#include <allIncludes.h>

class PhongSShader: public LambertShader
{
 public:
  PhongSShader(); //default constructor
  //useful constructor
  PhongSShader(Point3Dd amb, 
	      Point3Dd diff,
	      Point3Dd spec,
	      double specCo,
	      double trans,
	      Renderer * sceneRend);
  PhongSShader(PhongSShader&); //copy constructor
  PhongSShader* create(Renderer *); //generic Shader create()
  PhongSShader* clone(); //generic Shader clone()

  ~PhongSShader();

  Point3Dd getColor(Hit&);

  std::istream& PhongSShader::in(std::istream&);
  std::ostream& PhongSShader::out(std::ostream&);

 protected:
  double specularCoefficient;
  Point3Dd specular;
  Point3Dd getHalfwaySpecularColor(); //uses halfway speedup
  Point3Dd getSpecularColor(Light ** itVisibleLights, Hit &hitPoint); //uses the phong model
};

#endif















