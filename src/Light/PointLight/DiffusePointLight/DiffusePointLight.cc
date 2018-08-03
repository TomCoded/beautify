#include "DiffusePointLight.h"

DiffusePointLight::DiffusePointLight()
{
  power=1e3;
  location=Point3Dd(0.0,0.0,0.0);
}

DiffusePointLight::DiffusePointLight(DiffusePointLight &other)
{
  if(this!=&other)
    {
      location=other.location;
      power=other.power;
      specular=other.specular;
      diffuse=other.diffuse;
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
  double denom = diffuse.x+diffuse.y+diffuse.z;
  float xPow = (power * (diffuse.x / denom)) / numPhotons;
  float yPow = (power * (diffuse.y / denom)) / numPhotons;
  float zPow = (power * (diffuse.z / denom)) / numPhotons;
  Photon p;
  
  g_photonPower.x = xPow;
  g_photonPower.y = yPow;
  g_photonPower.z = zPow;

  int priorPhotons=map->getSize();
  
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
      
      //      std::cout << "__BEGIN_PHOTON_CHECK__\n";
      p = renderer->tracePhoton(p);
      //      std::cout << "__END_PHOTON_CHECK__\n";
      //add returned photon to map.
      if(!((p.r==p.g)&&(p.g==p.b)&&(p.g==0))) //if the photon has any power
	{
	  //	  std::cout << "Photon has power.\n";
	  map->addPhoton(p);
	  //	  std::cout << "Added Photon " << p <<std::endl;
	}
    }
  
  std::cout << "Light " << (int)this << " adds " <<
    map->getSize() - priorPhotons << " photons to map\n";

}

std::istream& DiffusePointLight::in(std::istream& is)
{
  char c;
  is >> c >> location;
  if(c!='(')
    {
      cerr << "Badformat for DiffusePointLight\n";
      cerr << "Format: (location,diffuse,specular,power)\n";
      exit(1);
    }
  is >> c >> diffuse;
  if(c!=',')
    {
      cerr << "Badformat for DiffusePointLight\n";
      cerr << "Format: (location,diffuse,specular,power)\n";
      exit(1);
    }
  is >> c >> specular;
  if(c!=',')
    {
      cerr << "Badformat for DiffusePointLight\n";
      cerr << "Format: (location,diffuse,specular,power)\n";
      exit(1);
    }
  is >> c >> power;
  if(c!=',')
    {
      cerr << "Badformat for DiffusePointLight\n";
      cerr << "Format: (location,diffuse,specular,power)\n";
      exit(1);
    }
  is >> c;
  if(c!=')')
    {
      cerr << "Badformat for DiffusePointLight\n";
      cerr << "Format: (location,diffuse,specular,power)\n";
      exit(1);
    }
  return is;
}

std::ostream& DiffusePointLight::out(std::ostream& o)
{
  o << '(' 
    << location 
    << ','
    << diffuse
    << ','
    << specular 
    << ',' 
    << power 
    << ')';
  return o;
}

std::istream& operator>>(std::istream &is, DiffusePointLight
&l)
{
  return l.in(is);
}

std::ostream& operator<<(std::ostream &o, DiffusePointLight &l)
{
  return l.out(o);
}
