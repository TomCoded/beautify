//Light.cc
//(C) 2002 Tom White

#include <Light/Light.h>

//destructor
Light::~Light() {}

istream& Light::in(istream&) {}

ostream& Light::out(ostream&) const {}

Point3Dd Light::getPower() const
{
  return power;
}

Point3Dd Light::setPower(Point3Dd& pow) {
  power = pow;
}

double Light::getDistance(const Point3Dd& point) const {
  return LIGHT_INF_DISTANCE;
}

void Light::addPhotonsToMap(int numPhotons,
			     PhotonMap *,
			     Renderer *
			     )
{
  //this light does not contribute any photons to the overall map;
  cerr << "No photon contribution from light source at " << this << endl;
}
