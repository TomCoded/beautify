#include "Photon.h"

std::ostream& operator<<(std::ostream& o, Photon& p)
{
  o << p.x << ' '
    << p.y << ' '
    << p.z << ' '

    << p.r << ' '
    << p.g << ' '
    << p.b << ' '

    << p.dx << ' '
    << p.dy << ' '
    << p.dz << ' '

    << p.flag << ' '
    << p.offset;
  return o;
}

std::istream& operator>>(std::istream& is, Photon& p)
{
  char c;

  is >> p.x 
     >> p.y 
     >> p.z 

     >> p.r 
     >> p.g 
     >> p.b 

     >> p.dx 
     >> p.dy 
     >> p.dz

     >> p.flag
     >> p.offset;

  return is;
}
