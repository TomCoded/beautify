//AllIncludes.h
//(C) 2002 Tom White
#ifndef ALLINCLUDES_H_
#define ALLINCLUDES_H_

#include <config.h>
#include <parallel_pm.h>
#include <iostream>
//#include <math.h>
#include <cmath>
#include <vector>
#include <Defs.h>
#include <Point3Dd.h>
#include <Point3Df.h>
#include <Point4Dd.h>
#include <Point4Df.h>
#include <Ray/Ray.h>
#include <Transform4Dd.h>
#include <TransformMaker/TransformMaker.h>
#include <Hit/Hit.h>
#include <Light/Light.h>
#ifndef COMPILINGLIGHT
  #include <Light/DirLight/DirLight.h>
  #include <Light/PointLight/PointLight.h>
#endif COMPILINGLIGHT
#include <Shader/Shader.h>
#ifndef COMPILINGSHADER
  #include <Shader/LambertShader/LambertShader.h>
#if 0
  #include <Shader/LambertShader/PhongShader/PhongShader.h>
  #include <Shader/LambertShader/PhongSShader/PhongSShader.h>
  #include <Shader/LambertShader/PhongSTShader/PhongSTShader.h>
  #include <Shader/LambertShader/PhongSRShader/PhongSRShader.h>
  #include <Shader/LambertShader/PhongSRTShader/PhongSRTShader.h>
#endif
#endif
#include <Shape/Shape.h>
#ifndef COMPILINGSHAPE
#include <Shape/Sphere/Sphere.h>
#include <Shape/Plane/Plane.h>
#include <Shape/TaperedCyl/TaperedCyl.h>
#endif COMPILINGSHAPE
#include <Surface/Surface.h>
#include <Camera/Camera.h>
#include <Scene/Scene.h>
#include <Renderer/Renderer.h>
#include <FunTransform4Dd.h>

#endif

