//Shader.h
//(C) 2002 Tom White

#ifndef SHADER_H_
#define SHADER_H_
#define COMPILINGSHADER

class Renderer;

#include <allIncludes.h>

class Shader
{
 public:
  virtual Shader* create(Renderer *) = 0;
  virtual Shader* clone() = 0;

  //destructor
  virtual ~Shader() = 0;

  virtual Point3Dd getColor(Hit&) = 0;

  //I/O functions
  virtual istream& in(istream&) = 0;
  virtual ostream& out(ostream&) = 0;

  double transparency;

 protected:
  int acceptShadows;
  int reflectionDepth;
  Renderer * sceneRenderer;
};

#undef COMPILINGSHADER
#endif


