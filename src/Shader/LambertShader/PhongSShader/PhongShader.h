#ifndef PHONGSHADER_H_
#define PHONGSHADER_H_

class Shader;
class LambertShader;

#include <allIncludes.h>

class PhongShader: public LambertShader
{
 public:
  PhongShader(); //default constructor
  //useful constructor
  PhongShader(Point3Dd amb, 
	      Point3Dd diff,
	      Point3Dd spec,
	      double specCo,
	      double trans,
	      Renderer * sceneRend);
  PhongShader(PhongShader&); //copy constructor
  PhongShader* create(Renderer *); //generic Shader create()
  PhongShader* clone(); //generic Shader clone()

  ~PhongShader();

  Point3Dd getColor(Hit);

  std::istream& PhongShader::in(std::istream&);
  std::ostream& PhongShader::out(std::ostream&);

 protected:
  double specularCoefficient;
  Point3Dd specular;
  Point3Dd getHalfwaySpecularColor(); //uses halfway speedup
  Point3Dd getSpecularColor(Light ** itVisibleLights, Hit &hitPoint); //uses the phong model
};

#endif















