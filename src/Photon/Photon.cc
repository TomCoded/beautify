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

inline Photon createPhoton(Point3Dd location,
		    Point3Dd incidentDir,
		    Point3Dd intensity
		    )
{
  Photon rv;
  rv.x = location.x;
  rv.y = location.y;
  rv.z = location.z;
  rv.r = intensity.x;
  rv.g = intensity.y;
  rv.b = intensity.z;
  rv.dx = incidentDir.x;
  rv.dy = incidentDir.y;
  rv.dz = incidentDir.z;
  return rv;
}
