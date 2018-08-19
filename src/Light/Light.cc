//Light.cc
//(C)  Tom White

#include <Light/Light.h>

//destructor
Light::~Light() {}

std::istream& Light::in(std::istream&) {}

std::ostream& Light::out(std::ostream&) const {}

double Light::getPower() const
{
  return power;
}

void Light::setPower(const double pow) {
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
  std::cerr << "No photon contribution from light source at " << this <<std::endl;
}
