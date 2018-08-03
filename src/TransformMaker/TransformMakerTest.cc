// testlinAlgLib.cc

// (C) 2002 Bill Lenhart

#include "TransformMaker/TransformMaker.h"

void print(const char * string, const Transform4Dd& t)
{
  std::cout << string << " = " << t <<std::endl;
}

int main(void) {

  print("xRotate 60 degrees", MakeXRotation(PI/3));
  print("yRotate 30 degrees", MakeYRotation(PI/6));
  print("zRotate 90 degrees", MakeZRotation(PI/2));
  print("translate ", MakeTranslation(Point3Dd(2,-1,3)));
  print("translate ", MakeTranslation(2,-1,3));
  print("scale ", MakeScale(Point3Dd(2,-1,3)));
  print("scale ", MakeScale(2,-1,3));

  return 0;
}
