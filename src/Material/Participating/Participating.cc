#include <stdlib.h>
#include <Defs.h>
#include <Material/Participating/Participating.h>

Participating::Participating()
{
  participating=true;
}

Participating::Participating(Participating &other)
{
  if(this!=&other)
    {
      ambient=other.ambient;
      specular=other.specular;
      diffuse=other.diffuse;
      specCo=other.specCo;
      participating=other.participating;
    }
}

CLONEMETHOD(Participating);

Participating::~Participating()
{}

int Participating::nRoulette(Photon &p)
{
  double dRand = drand48();
  //don't do anything fancy

  if(dRand < scatCo)
    return SCATTER;
  return STEP;
}

void Participating::DoBRDF(Photon &p)
{
  //Adjust photon's intensity to account for the probability of its
  //  survival to this point in the medium
  p.r = p.r * (1-scatCo);
  p.g = p.g * (1-scatCo);
  p.b = p.b * (1-scatCo);
}

istream& Participating::in(istream &is)
{
  char ch;
  is >> ch >> ambient;
  if(ch!='(')
    {
      cerr << "Bad format for Participating: '(' not found!\n";
      exit(1);
    }
  is >> ch >> diffuse;
  if(ch!=',')
    {
      cerr << "Bad format for Participating: ',' not found!\n";
      exit(1);
    }
  is >> ch >> specular;
  if(ch!=',')
    {
      cerr << "Bad format for Participating: ',' not found!\n";
      exit(1);
    }
  is >> ch >> specCo;
  if(ch!=',')
    {
      cerr << "Bad format for Participating: ',' not found!\n";
      exit(1);
    }
  is >> ch >> scatCo;
  if(ch!=',')
    {
      cerr << "Bad format for Participating: ',' not found!\n";
      exit(1);
    }
  is >> ch;
  if(ch!=')')
    {
      cerr << "Bad format for Participating: ')' not found!\n";
      cerr << ch << "found instead\n";
      exit(1);
    }
  return is;
}

ostream& Participating::out(ostream &o)
{
  o << '('
    << ambient << ','
    << diffuse << ','
    << specular << ','
    << specCo << ','
    << scatCo << ')';
  return o;
}

istream& operator>>(istream& is, Participating&p)
{
  return p.in(is);
}

ostream& operator<<(ostream& o, Participating &p)
{
  return p.out(o);
}
