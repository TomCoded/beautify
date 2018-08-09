//PhongSRShader.h
//(C) 2002 Tom White
#ifndef PHONGSRSHADER_H_
#define PHONGSRSHADER_H_

class Shader;
class LambertShader;

#include <allIncludes.h>

class PhongSRShader: public LambertShader
{
 public:
  PhongSRShader(); //default constructor
  //useful constructor
  PhongSRShader(Point3Dd amb, 
		Point3Dd diff,
		Point3Dd spec,
		double specCo,
		double trans,
		double reflection,
		Renderer * sceneRend);
  PhongSRShader(PhongSRShader&); //copy constructor
  PhongSRShader* create(Renderer *); //generic Shader create()
  PhongSRShader* clone(); //generic Shader clone()

  ~PhongSRShader();

  Point3Dd getColor(Hit&);

  std::istream& PhongSRShader::in(std::istream&);
  std::ostream& PhongSRShader::out(std::ostream&);

 protected:
  double reflection;
  double specularCoefficient;
  Point3Dd specular;
  Point3Dd getHalfwaySpecularColor(); //uses halfway speedup
  Point3Dd getSpecularColor(Light ** itVisibleLights, Hit &hitPoint); //uses the phong model
};

#endif















