//(C) 2002 Tom White
#ifndef PHONGSTSHADER_H_
#define PHONGSTSHADER_H_

class Shader;
class LambertShader;

#include <allIncludes.h>

class PhongSTShader: public LambertShader
{
 public:
  PhongSTShader(); //default constructor
  //useful constructor
  PhongSTShader(Point3Dd amb, 
	      Point3Dd diff,
	      Point3Dd spec,
	      double specCo,
	      double trans,
	      Renderer * sceneRend);
  PhongSTShader(PhongSTShader&); //copy constructor
  PhongSTShader* create(Renderer *); //generic Shader create()
  PhongSTShader* clone(); //generic Shader clone()

  ~PhongSTShader();

  Point3Dd getColor(Hit&);

  istream& PhongSTShader::in(istream&);
  ostream& PhongSTShader::out(ostream&);

 protected:
  double specularCoefficient;
  Point3Dd specular;
  Point3Dd getHalfwaySpecularColor(); //uses halfway speedup
  Point3Dd getSpecularColor(Light ** itVisibleLights, Hit &hitPoint); //uses the phong model
};

#endif















