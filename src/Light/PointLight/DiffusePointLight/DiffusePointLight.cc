#include "DiffusePointLight.h"

DiffusePointLight::DiffusePointLight()
{
  power=Point3Dd(0.5,0.5,0.5);
  location=Point3Dd(0.0,0.0,0.0);
}

DiffusePointLight::DiffusePointLight(DiffusePointLight &other)
{
  if(this!=&other)
    {
      location=other.location;
      power=other.power;
      specular=other.specular;
    }
}

DiffusePointLight::~DiffusePointLight()
{}

//#include <sys/time.h>

void DiffusePointLight::addPhotonsToMap(int numPhotons,PhotonMap * map,
					Renderer * renderer)
{
  //power per photon is power / number photons emitted
  float dx, dy, dz;
  float xPow = power.x / numPhotons;
  float yPow = power.y / numPhotons;
  float zPow = power.z / numPhotons;
  Photon p;
  for(int nEmitted=0;
      nEmitted < numPhotons;
      nEmitted++)
    {
      do 
	{
	  dx = (drand48() - 0.5)*2;
	  dy = (drand48() - 0.5)*2;
	  dz = (drand48() - 0.5)*2;
	} while ((SQR(dx)+SQR(dy)+SQR(dz)) > 1);
      //Renderer::tracePhoton;
      //all Photons emitted by the light have the same power.
      //this keeps us from wasting time with low-power photons.
      //The power has to be reset b/c we adjust it on
      //surface intersection to account for the 
      //probability of destruction.
      p.x = location.x;
      p.y = location.y;
      p.z = location.z;
      p.r = xPow;
      p.g = yPow;
      p.b = zPow;
      p.dx = dx;
      p.dy = dy;
      p.dz = dz;
      //      cout << "__BEGIN_PHOTON_CHECK__\n";
      p = renderer->tracePhoton(p);
      //      cout << "__END_PHOTON_CHECK__\n";
      //add returned photon to map.
      if(!((p.r==p.g)&&(p.g==p.b)&&(p.g==0))) //if the photon has any power
	{
	  //	  cout << "Photon has power.\n";
	  map->addPhoton(p);
	}
    }
}

istream& DiffusePointLight::in(istream& is)
{
  char c;
  is >> c >> power;
  if(c!='(')
    {
      cerr << "Badformat for DiffusePointLight\n";
      cerr << "Format: (power,location,specular)\n";
      exit(1);
    }
  is >> c >> location;
  if(c!=',')
    {
      cerr << "Badformat for DiffusePointLight\n";
      cerr << "Format: (power,location,specular)\n";
      exit(1);
    }
  is >> c >> specular;
  if(c!=',')
    {
      cerr << "Badformat for DiffusePointLight\n";
      cerr << "Format: (power,location,specular)\n";
      exit(1);
    }
  is >> c;
  if(c!=')')
    {
      cerr << "Badformat for DiffusePointLight\n";
      cerr << "Format: (power,location,specular)\n";
      exit(1);
    }
  return is;
}

ostream& DiffusePointLight::out(ostream& o)
{
  o << '(' << power << ','
    << location << ',' 
    << specular << ')';
  return o;
}

istream& operator>>(istream &is, DiffusePointLight
&l)
{
  return l.in(is);
}

ostream& operator<<(ostream &o, DiffusePointLight &l)
{
  return l.out(o);
}
