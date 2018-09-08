#include <stdlib.h>
#include <Defs.h>
#include <math.h>
#include <TransformMaker/TransformMaker.h>
#include <Material/Participating/Participating.h>
#include <Point4Dd.h>

Participating::Participating():
  scatCo(0.5,0.5,0.5),
  absorbCo(0.1,0.1,0.1),
  scatX(0),scatY(0),scatZ(0),
  absX(0),absY(0),absZ(0)
{}

Participating::Participating(Participating &other)
{
  if(this!=&other)
    {
      ambient=other.ambient;
      specular=other.specular;
      diffuse=other.diffuse;
      specCo=other.specCo;
      scatCo=other.scatCo;
      absorbCo=other.absorbCo;
      if(other.absX) {
	absX = other.absX->clone();
	absY = other.absY->clone();
	absZ = other.absZ->clone();
      }
      if(other.scatX) {
	scatX = other.scatX->clone();
	scatY = other.scatY->clone();
	scatZ = other.scatZ->clone();
      }
    }
}

Point3Dd Participating::phaseFunction(const Point3Dd &in,
				      const Point3Dd &out
				      ) const {
  //for all homogenous media
  // ( 1 - k^2) / (4pi((1+kcos(theta))^2))
  // left * ( 1 / (1 + k * cos (theta) )^2)
  double cost = in.dot(out) / (in.norm() * out.norm());
  double denom = 1 - cost * greenK;
  denom *= denom;
  double result = left*denom;
  //dampen effect
  if(result < 1) {
    result += (1.0-result)/2.0;
  } else {
    result -= (result-1.0)/(2.0);
  }
  //for visual reasons
  return Point3Dd(result,result,result);
}

CLONEMETHOD(Participating);

Participating::~Participating()
{
  delete scatX, scatY, scatZ;
  delete absX, absY, absZ;
}

bool Participating::participates() const {
  return true;
}

int Participating::nRoulette(Photon &p)
{
  Point3Dd extinctCo;

  double dRand = drand48();
  //don't do anything fancy

  //evalute scattering and extinction coefficients at this x,y,z location
  copyScatCo(scatCo, p.x, p.y, p.z);
  copyExtinctCo(extinctCo, p.x, p.y, p.z);

  //use average scattering albedo (mean ratio of scattering to
  //extinction coefficient) across color channels

  double probScatter = (
    scatCo.x / extinctCo.x + 
    scatCo.y / extinctCo.y + 
    scatCo.z / extinctCo.z
    ) / 3;

  if(dRand <= probScatter) {
    p.r = (p.r * scatCo.x) / probScatter;
    p.g = (p.g * scatCo.y) / probScatter;
    p.b = (p.b * scatCo.z) / probScatter;
    return SCATTER;
  }
  return ABSORPTION;
}

//picks a new outgoing direction according to phase function,
//then adjusts intensity in each color channel.
void Participating::DoBRDF(Photon &p)
{
  //Isotropic Scattering yields a phase function of 1/4pi
  //  p.r = p.r / (4 * PI);
  //  p.g = p.g / (4 * PI);
  //  p.b = p.b / (4 * PI);

#if 0
  //Now adjust photon direction, in this case isotropically
  Point3Dd newDir(drand48()*2-1
		  ,drand48()*2-1
		  ,drand48()*2-1
		  );
#endif

  //Now adjust photon direction, using Henyey-Greenstein function
  double rand1 = drand48();
  double phi = 360.0 * drand48();
  double theta = acos( (2.0*rand1 + greenK - 1.0) / (2.0 * greenK * rand1 - greenK + 1.0) );

  //#ifdef DEBUG_BUILD
#if 0
  //generate mean theta info
  double ttemp=0.0;
  for(int i=0; i<1000; i++) {
    rand1 = drand48();
    ttemp += acos( (2.0*rand1 + greenK - 1.0) / (2.0 * greenK * rand1 - greenK + 1.0) );
  }
  std::cerr << "Mean theta for " << greenK << " = " << ttemp/1000 <<std::endl;
#endif

  //Dump incoming photon dir into 4d point
  Point3Dd nDir(p.dx,p.dy,p.dz);
  Point3Dd xhat(1.0,0.0,0.0);

  //rotate by theta about a std::vector perpendicular to the incoming photon
  //create vector perpendicular to x^ and incident dir. Rotate incident direction around that and then around the original incident direfction.
  Point3Dd cross = nDir.cross(xhat);
  
  Transform4Dd thetaTrans = MakeRotation(theta,cross);
  Transform4Dd phiTrans = MakeRotation(phi,nDir);
  Point4Dd newDir(nDir,0.0);
  newDir = thetaTrans * newDir;
  newDir = phiTrans * newDir;

  newDir.normalize();

  p.dx = newDir.x;
  p.dy = newDir.y;
  p.dz = newDir.z;

  //Now compute the impact of the phase function on intensity
  //left = (1 - greenK*greenK) / (4 * PI);
  double denom = 1 + greenK * cos(theta);
  double modifier = left * (1.0 / (denom * denom));
  //#ifdef DEBUG_BUILD
#if 0
  std::cerr << "phase function (" << theta << "): " 
       << modifier 
       <<std::endl;
#endif
  p.r *= modifier;
  p.g *= modifier;
  p.b *= modifier;
}

Point3Dd Participating::getScatCo(const Point3Dd &loc) const {
  return Point3Dd(scatX->eval(loc.x),
		  scatY->eval(loc.y),
		  scatZ->eval(loc.z));
}

Point3Dd Participating::getAbsorbCo(const Point3Dd &loc) const {
  return Point3Dd(absX->eval(loc.x),
		  absY->eval(loc.y),
		  absZ->eval(loc.z));
}

Point3Dd Participating::getExtinctCo(const Point3Dd &loc) const {
  return Point3Dd(absX->eval(loc.x) + scatX->eval(loc.x),
		  absY->eval(loc.y) + scatY->eval(loc.y),
		  absZ->eval(loc.z) + scatZ->eval(loc.z));
}

Point3Dd Participating::getScatCo(const double x,
				  const double y,
				  const double z) const {
  return Point3Dd(scatX->eval(x),
		  scatY->eval(y),
		  scatZ->eval(z));
}

Point3Dd Participating::getAbsorbCo(const double x, 
				    const double y, 
				    const double z) const {
  return Point3Dd(absX->eval(x),
		  absY->eval(y),
		  absZ->eval(z));
}

Point3Dd Participating::getExtinctCo(const double x, 
				     const double y, 
				     const double z) const {
  return Point3Dd(absX->eval(x) + scatX->eval(x),
		  absY->eval(y) + scatY->eval(y),
		  absZ->eval(z) + scatZ->eval(z));
}

void Participating::copyScatCo(Point3Dd &outparam,
		const double x,
		const double y,
		const double z) const {
  outparam.x=scatX->eval(x);
  outparam.y=scatY->eval(y);
  outparam.z=scatZ->eval(z);
}

void Participating::copyScatCo(Point3Dd &outparam,
		const Point3Dd &loc) const {
  outparam.x=scatX->eval(loc.x);
  outparam.y=scatY->eval(loc.y);
  outparam.z=scatZ->eval(loc.z);
}

void Participating::copyExtinctCo(Point3Dd &outparam,
		const double x,
		const double y,
		const double z) const {
  outparam.x=scatX->eval(x)+absX->eval(x);
  outparam.y=scatY->eval(y)+absY->eval(y);
  outparam.z=scatZ->eval(z)+absZ->eval(z);
}

void Participating::copyExtinctCo(Point3Dd &outparam,
		const Point3Dd &loc) const {
  outparam.x=scatX->eval(loc.x)+absX->eval(loc.x);
  outparam.y=scatY->eval(loc.y)+absY->eval(loc.y);
  outparam.z=scatZ->eval(loc.z)+absZ->eval(loc.z);
}


std::istream& Participating::in(std::istream &is)
{
  char c;
  std::string formatErr("Bad format for Participating");
  is >> c;

  SumFunNode parser;
  FORMATTEST(c,'(',formatErr)

  scatX = parser.in(is);
  scatY = parser.in(is);
  scatZ = parser.in(is);

  is >> c;
  FORMATTEST(c,',',formatErr)

  absX = parser.in(is);
  absY = parser.in(is);
  absZ = parser.in(is);

  is >> c;
  FORMATTEST(c,',',formatErr)

  is >> greenK;
  is >> c;
  FORMATTEST(c,')',formatErr)

  left = (1 - greenK*greenK) / (4 * PI);

  return is;
}

std::ostream& Participating::out(std::ostream &o)
{
  o << '('
    << *scatX 
    << *scatY 
    << *scatZ << ','
    << *absX
    << *absY
    << *absZ
    << ','
    << greenK
    << ')';
  return o;
}

std::istream& operator>>(std::istream& is, Participating&p)
{
  return p.in(is);
}

std::ostream& operator<<(std::ostream& o, Participating &p)
{
  return p.out(o);
}
