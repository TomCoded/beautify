//Renderer.cc
//(C) Tom White

#include <parallel_pm.h>
#include <allIncludes.h>
#include <PhotonMap/PhotonMap.h>

//generate a bunch of stats
//#define DEBUG_BUILD

//only display direct interactions w/ volume photon map
//#define VOLMAP_ONLY

//disable specular lighting
//#define NOSPECULAR

//disable multiple scattering effect in participating media
//#define SINGLESCATTER

extern int antialias;
extern int g_specModel;

//default photon power
Point3Dd g_photonPower;
//no photons below this power used
Point3Dd g_photonPowerLow;

#define USEFUL(p) ((p.r>g_photonPowerLow.x)||(p.g>g_photonPowerLow.y)||(p.b>g_photonPowerLow.z))

Renderer::Renderer():
  myScene(0), ambient(0.5,0.5,0.5), currentCamera(0), pMap(0), 
  pVolMap(0), storeDirectLight(false), storeVolumeDirectLight(false)
{}

Renderer::Renderer(Renderer& other)
{
  myScene = other.myScene;
  currentCamera = other.currentCamera;
  ambient = other.ambient;
  pMap = other.pMap;
  pVolMap=other.pVolMap;
  storeDirectLight=other.storeDirectLight;
  storeVolumeDirectLight=other.storeVolumeDirectLight;
} 

Renderer::Renderer(Scene * myScene):
  ambient(0.5,0.5,0.5), currentCamera(0), pMap(0),
  pVolMap(0), storeDirectLight(false), storeVolumeDirectLight(false)
{
  setScene(myScene);
}

void Renderer::setScene(Scene * myScene)
{
  this->myScene = myScene;
  ambient = myScene->ambient;
  currentCamera = myScene->getCamera();
}

//destructor
Renderer::~Renderer()
{
  delete pMap;
  delete pVolMap;
}

bool Renderer::storesDirectLight(const bool storeDirectLight) {
  this->storeDirectLight=storeDirectLight;
  return this->storeDirectLight;
}

bool Renderer::storesVolumeDirectLight(const bool storeVolumeDirectLight) {
  this->storeVolumeDirectLight=storeVolumeDirectLight;
  return this->storeVolumeDirectLight;
}

//Management functions

extern PhotonMap * g_map;
void Renderer::newMap() {
  if(pMap) {
    delete pMap;
  }
  if(pVolMap)
    delete pVolMap;
  pVolMap = new PhotonMap();
  pMap = new PhotonMap();
  g_map = pMap;
}

PhotonMap * Renderer::map()
{
  int photons = photonsEmittedCount ?
    photonsEmittedCount :
    DEFAULT_PHOTONS;
  map(photons);
}

PhotonMap * Renderer::getVolMap()
{
  return pVolMap;
}

//map() runs the photon mapper
//nPhotons is the number of photons to create from lights, total.
PhotonMap * Renderer::map(int nPhotons)
{
  photonsEmittedCount=nPhotons;
  static int light_pow_set=0;
#ifdef PARALLEL
  int rank, nodes;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&nodes);
#endif

  newMap();
  
  std::shared_ptr<std::vector<std::shared_ptr<Light>>> lights
    = myScene->getLights();
  auto itLights = lights->begin();
  int nLights = lights->size();
  int nPhotonsForThisLight=0;
  double totalPower=0.0;
  double lightPower=0.0;
  for(;itLights!=lights->end(); itLights++)
    { //determine total power of lights in the scene
      totalPower+=(*itLights)->getPower();
    }

  for(itLights=lights->begin();
      itLights!=lights->end();
      itLights++)
    { //for each light in the scene
      //calculate the number of photons needed

#ifdef DEBUG_BUILD
#ifdef PARALLEL
      std::cout << rank << ':';
#endif
#endif
      lightPower=((*itLights)->getPower());
      nPhotonsForThisLight=(int)(nPhotons*(lightPower/totalPower));

#ifdef PARALLEL
      if(g_parallel) {
	if(!light_pow_set) {
	  //We want each light to emit photons of power
	  // nPhotonsForThisLight, but we don't want to divide the
	  //total light power by the number of photons, we want to
	  // divide it by the number of photons times the number of nodes.
	  
	  // Note nPhotons is ALREADY adjusted to
	  // reflect just the number of photons this node
	  // will emit.
	  double pow = (*itLights)->getPower();
	  pow /= nodes;
	  (*itLights)->setPower(pow);
	}
      }
#endif

      //Have each light emit photons
      (*itLights)->addPhotonsToMap(nPhotonsForThisLight,
				   this
				   );
    }
  
  light_pow_set=1;
  g_photonPowerLow = g_photonPower;
  g_photonPowerLow.x /= 4.0;
  g_photonPowerLow.y /= 4.0;
  g_photonPowerLow.z /= 4.0;

  return pMap;
}

Point3Dd Renderer::directLightingLookingAlong(Ray * sampleRay)
{
  return directLightingLookingAlong(sampleRay,myScene->getSurfaces());
}

Point3Dd Renderer::directLightingLookingAlong(
			    Ray * sampleRay,
			    std::shared_ptr<std::vector<std::shared_ptr<Surface>>> surfaces
			    )
{
  std::vector<std::shared_ptr<Surface>> intersectedSurfaces;
  Point3Dd color(0,0,0);
  std::shared_ptr<Surface> closestSurface = 0;
  std::shared_ptr<Surface> farthestSurface = 0;
  double tClose = 100000000; double tLast = tClose; double tFar=0;

  if(recursionDepth>1) return Point3Dd(0,0,0);
  else recursionDepth++;

  for(auto itSurf = surfaces->begin(); itSurf != surfaces->end(); itSurf++)
    { //iterate through itSurf, to find the closest intersected
      //      surface.
      Point4Dd raySrc =
	(*itSurf)->transWorldToLocal*
	(*itSurf)->translateWorldToLocal*sampleRay->src;
      Point4Dd rayDir = 
	(*itSurf)->transWorldToLocal*
	(*itSurf)->translateWorldToLocal*sampleRay->dir;
      Ray localSampleRay(raySrc.x, raySrc.y, raySrc.z, 
			 rayDir.x, rayDir.y, rayDir.z);
      tLast = (*itSurf)->tScratch
	= (*itSurf)->surShape->closestIntersect(localSampleRay);
      if(tLast!=-1) // intersected something
	{ //add to std::list of intersected surfaces
	  //scan to find proper place, by t-Value
	  if(intersectedSurfaces.size()==0)
	    {
	      intersectedSurfaces.push_back(*itSurf);
	    }
	  else
	    { //find the right insertion spot
	      intersectedSurfaces.push_back(*itSurf);
	      //update indexes for later values.
	    }
	  if(tLast < tClose)
	    { 
	      closestSurface = (*itSurf);
	      tClose = tLast;
	      if(tClose>tFar) 
		{
		  tFar=tClose;
		  farthestSurface = (*itSurf);
		}
	    }
	}
    }  
  if(closestSurface) //we hit something
    {
      if(!closestSurface->participates()) {
	Point4Dd raySrc =
	  closestSurface->transWorldToLocal*
	  closestSurface->translateWorldToLocal*sampleRay->src;
	Point4Dd rayDir = 
	  closestSurface->transWorldToLocal *
	  closestSurface->translateWorldToLocal*sampleRay->dir;
	Ray localSampleRay(raySrc.x, raySrc.y, raySrc.z, 
			   rayDir.x, rayDir.y, rayDir.z);
	  
	Ray normal = //get normal in local coordinate system
	  closestSurface->surShape->getNormal(localSampleRay);
	//only use transLocalToWorldNormal for directional changes, when
	//and if they are necessary.
	//normal source has to be translated differently
	Point4Dd tempPoint(normal.src);
	tempPoint = closestSurface->transLocalToWorld*tempPoint;
	normal.src=tempPoint;
	tempPoint = normal.dir;
	tempPoint = closestSurface->transLocalToWorldNormal*tempPoint;
	normal.dir = tempPoint;
	//now we have the world normal
	Point4Dd hp4 = sampleRay->GetPointAt(tClose);
	Hit hit(Point3Dd(hp4.x,hp4.y,hp4.z),
		closestSurface,
		normal,
		Ray(*sampleRay),
		tClose
		);
	color=
	  (closestSurface->surShader->getColor(hit));
	std::cout << "Direct color: " << color << std::endl;
      } else {
	//participating medium.
	//do not add direct lighting since will be computed elsewhere
      }
    }
  recursionDepth--;
  return color;
}

//I/O functions
std::istream& Renderer::in(std::istream& in)
{
  UNIMPLEMENTED("Renderer.in");
}

std::ostream& Renderer::out(std::ostream& out)
{
  UNIMPLEMENTED("Renderer.out")
}

std::shared_ptr<std::vector<std::shared_ptr<Light>>> Renderer::getAllLights()
{
  return myScene->getLights();
}

//gets lights in the scene, that are visible to a certain point.
//the intensity of the light source decreases linearly 
//with the transparency of the surfaces through which
//the light ray passes.
//FIXME: Has an issue with transparency. Currently disabled with migration to smart pointers for lights.
std::shared_ptr<std::vector<std::shared_ptr<Light>>> Renderer::getApparentLights(Point3Dd point)
{
  std::shared_ptr<std::vector<std::shared_ptr<Light>>> lightsToReturn
    = std::make_shared<std::vector<std::shared_ptr<Light>>>();
  double tLast, tClose, tPoint;
  double transparency=1;
  Point4Dd raySrc, rayDir; 
  Ray localSampleRay; Ray sampleRay;

  auto itLights = myScene->getLights()->begin();
  auto itSurfaces = myScene->getSurfaces()->begin();
  auto itLastLight = myScene->getLights()->end();
  auto lastSurface = myScene->getSurfaces()->end();

  for(;itLights != itLastLight; itLights++)
    { //for each light in the scene
      //calculate t-point of current point
      sampleRay = Ray((*itLights)->getRayFromLightToward(point));
      tPoint = sampleRay.TofClosestPoint(Point4Dd(point,0));
      tClose = -1;
      for(int done=false;(!done)&&(itSurfaces != lastSurface); itSurfaces++)
	{ //we find out if any surfaces intersect the ray
	  raySrc=
	    (*itSurfaces)->transWorldToLocal*
	    (*itSurfaces)->translateWorldToLocal*sampleRay.src;
	  rayDir=
	    (*itSurfaces)->transWorldToLocal*
	    (*itSurfaces)->translateWorldToLocal*sampleRay.dir;
	  localSampleRay= Ray(raySrc.x, raySrc.y, raySrc.z, 
			      rayDir.x, rayDir.y, rayDir.z);
	  tLast=(*itSurfaces)->surShape->closestIntersect(localSampleRay);
	  if(tLast>0)
	    { //we intersected a shape.
	      if(tLast < tPoint)
		{ //it shades the light
		  tClose = tLast;
		  transparency+=(*itSurfaces)->surShader->transparency;
		  if(transparency==0)
		    done=true;
		}
	    }
	}
      //      if((tClose==-1)&&(transparency!=0)) //the light is visible
      //	lightsToReturn->push_back(((**itLights).transparent(transparency)));
      // transparency function does not work with shared pointers.
    }
  return lightsToReturn;
}

std::shared_ptr<std::vector<std::shared_ptr<Light>>> Renderer::getVisibleLights(Point3Dd point)
{
  std::shared_ptr<std::vector<std::shared_ptr<Light>>> lightsToReturn
    = std::make_shared<std::vector<std::shared_ptr<Light>>>();
  auto itLights = myScene->getLights()->begin();
  auto itSurfaces = myScene->getSurfaces()->begin();
  auto itLastLight = myScene->getLights()->end();
  auto lastSurface = myScene->getSurfaces()->end();
  Point4Dd raySrc, rayDir; 
  double tLast, tClose, tPoint;
  Ray localSampleRay; Ray sampleRay;
  for(;itLights != itLastLight; itLights++)
    { //for each light in the scene
      //calculate t-point of current point
      sampleRay = Ray((*itLights)->getRayFromLightToward(point));
      tPoint = sampleRay.TofClosestPoint(Point4Dd(point,0));
      tClose = -1;
      for(int done=false;(!done)&&itSurfaces!=lastSurface; itSurfaces++)
	{ //we find out if any surfaces intersect the ray
	  raySrc=
	    (*itSurfaces)->transWorldToLocal*
	    (*itSurfaces)->translateWorldToLocal*sampleRay.src;
	  rayDir=
	    (*itSurfaces)->transWorldToLocal*
	    (*itSurfaces)->translateWorldToLocal*sampleRay.dir;
	  localSampleRay= Ray(raySrc.x, raySrc.y, raySrc.z, 
			      rayDir.x, rayDir.y, rayDir.z);
	  tLast=(*itSurfaces)->surShape->closestIntersect(localSampleRay);
	  if(tLast>0.01)
	    { //we intersected a shape.
		{ //it shades the light
		  tClose = 1;
		  done=true;
		}
	    }
	}
      if(tClose==-1) //the light is visible
	lightsToReturn->push_back(*itLights);
    }
  return lightsToReturn;
}

/*
  Photon& tracePhoton &p;
  Takes a photon (By reference)
   - Photon's incident direction encoded in p.dx,.dy,.dz
   - Photon's current position encoderd in p.x, y, z
   We transmit the photon to the nearest intersected surface.
   Then we perform Russian Roulette.  
   And call this recursively if the photon continues to move.
 */
//for when photons should not be added to the global photon map
#define NULL_PHOTON p.r = p.g = p.b = 0
void Renderer::tracePhoton(Photon &p, int recurse /*=0*/)
{
#ifdef DEBUG_BUILD
  static int total=0; static int intersected=0;
  total++;
#endif
  //If we're beyond maxdepth, back out
  if(recurse > maxdepth) {
    NULL_PHOTON;
    return;
  }

  //First scan through all surfaces for the closest one.
  double tClose, tCurrent;
  tClose = 1000000;
  auto itSurfaces = myScene->getSurfaces()->begin();  
  auto lastSurface = myScene->getSurfaces()->end();
  std::shared_ptr<Surface> closestSurface = NULL;
  Ray * sampleRay = new Ray(p.x,
			    p.y,
			    p.z,
			    p.dx,
			    p.dy,
			    p.dz
			    );
  Point3Dd location(p.x,p.y,p.z);
  while(itSurfaces!=lastSurface)
    { //for all surfaces in scene
      //determine the t-Value of the closest intersect
      tCurrent = (*itSurfaces)->closestIntersect(*sampleRay);
      //      std::cout << "Implicit value of " << location << " is " <<
      //	(*itSurfaces)->implicit(location) <<std::endl;
      if((tCurrent!=-1)&&(tCurrent < tClose)&&(0<((*itSurfaces)->implicit(location))))
	{
	  tClose=tCurrent;
	  closestSurface = *itSurfaces;
	}
      itSurfaces++;
    }
  //Now the closest surface, if it exists, is in closestSurface
  // we want to perform Russian roulette, and 
  //  call this recursively if the photon is not absorbed.
#ifdef DEBUG_BUILD
      if(!(total % 20000))
	std::cout << "Current ratio of Hits/Misses: " << intersected << '/'
	     << total <<std::endl;
#endif

  if(closestSurface)
    { //we hit something
#ifdef DEBUG_BUILD
      intersected++;
      std::cout << "tClose = " << tClose <<std::endl;
      if (tClose < 0) 
        std::cout << "NEGTCLOSE\n";
      std::cout << "point at tClose = " << sampleRay->GetPointAt(tClose) <<
        std::endl;
      std::cout << "BUMPDISTANCE = " << BUMPDISTANCE <<std::endl;
      std::cout << "point at tClose - BUMPDISTANCE = " <<
	sampleRay->GetPointAt(tClose-BUMPDISTANCE) <<std::endl;
      std::cout << "point at tClose + BUMPDISTANCE = " <<
	sampleRay->GetPointAt(tClose+BUMPDISTANCE) <<std::endl;
      std::cout << "SampleRay = " << *sampleRay <<std::endl;
#endif
      Point4Dd ip4 = sampleRay->GetPointAt(tClose-BUMPDISTANCE);
      Point3Dd iPoint(ip4.x,ip4.y,ip4.z);
      Point3Dd nPoint = closestSurface->getNormalAt(p.x,
						    p.y,
						    p.z,
						    p.dx,
						    p.dy,
						    p.dz
						    );

      //nDoRoulette runs Russian roulette on the photon.
      //If it is reflected, it adjusts the power of the photon in 
      //each color band to account for the probability
      // of the photon's survival.
      if(!closestSurface->participates())
	{ //if not a participating medium
	  switch(closestSurface->nDoRoulette(p))
	    {
	    case DIFFUSE_REFLECTION:
	      
	      if((p.r > 0.0)
		 ||(p.g>0.0)
		 ||(p.b>0.0)
		 )
		{
		    Photon p2 = p;
		    
		    p2.x=iPoint.x;
		    p2.y=iPoint.y;
		    p2.z=iPoint.z;
		    //just using lambertian model for now.
		    //		  std::cout << "pre -BRDF p2 is " << p2 <<std::endl;
		    //		  closestSurface->DoBRDF(p2);
		    //		  std::cout << "post-BRDF p2 is " << p2 <<std::endl;
		    p=p2;
		    if(storeDirectLight||p.bounced)
		      pMap->addPhoton(p2);

		    resetIncidentDir(p,nPoint);
		    p.bounced=true;
		    
		    return tracePhoton(p,++recurse);
		}

	      //first, de-allocate sample ray.
	      delete sampleRay;
	      break;

	case SPECULAR_REFLECTION:
	      NULL_PHOTON;
	      delete sampleRay;
	      return;
	      break;
	    case ABSORPTION:
	      NULL_PHOTON;
	      delete sampleRay;
	      return;
	      break;
	    default: 
	      std::cerr << "Error: unknown Russian Roulette result in Renderer.cc\n";
	      exit(1);
	    }
	}
      else
	{ //volume participates; behave a little differently.
	  //first, set hitpoint to be just *inside* the medium

	  Point4Dd ip4 = sampleRay->GetPointAt(tClose+BUMPDISTANCE);
	  p.x = ip4.x;
	  p.y = ip4.y;
	  p.z = ip4.z;

	  //send photon inside medium
	  tracePhotonInParticipatingMedium(p,closestSurface);
	}
    }
  delete sampleRay;
  NULL_PHOTON;
  return;
}
#undef NULL_PHOTON

Point3Dd Renderer::estimateExtinctionCoefficient(std::shared_ptr<Surface> surface,
					     Participating * material,
					     Photon &p,
					     Point3Dd &location,
					     int extinctionSamplesPerStep
					     ) {
  //distance to next event is 
  //  d(x_p) = - log (zdeta)/ (sigma bar sub t of x_p)
  // for random zdeta from (0,1].
  //  pdf(d) = e^(-sigma_t(x_p)d
  // for homogenous media, could just set sigma bar sub t of x_p to the medium's extinction co.
  // for heterogenous media, we guess. One option is set parameter to the extinction coefficient of the scattering event. Add check sigma bar sub t>0. (Other options would be compute average over whole heterogenous media and treat as homogenous, or take small steps to make estimate). We use a variety of the third option, using extinction coefficient at first point to guess about step size and getting mean of extinction coefficients at current location, final estimate step, and in the middle.
      
  //use ray marching heuristic to guesstimate mean extinction
  //coefficient over non-homogenous media

  //cache of the extinction coefficients of medium in current location
  Point3Dd extinctCache(0.0,0.0,0.0);
  //used to hold location where we're gathering extinction coefficient data
  Point3Dd extinctLocCheck;
  Point3Dd tempLoc;
  double meanExtinct;
  double divisor=0;
  double stepSize=0.0;
  double randVal = drand48();
  Point3Dd step(p.dx,p.dy,p.dz);
  
  for(int i=0; i<extinctionSamplesPerStep; i++) {
    step.normalize();
    if(i)
      step*=stepSize*(((double)i)/(double)(extinctionSamplesPerStep));
    tempLoc = i ? location+step : location;
    
    if(surface->contains(tempLoc)) {
      material->copyExtinctCo(extinctLocCheck,tempLoc);
      extinctCache+=extinctLocCheck;
      ++divisor;
    } else break;

    if(!i) {
      meanExtinct = extinctCache.x*extinctCache.y*extinctCache.z / 3;
      stepSize = - ( log(randVal) ) / meanExtinct;
    }
  }
      
  //take mean of our sample points
  extinctCache.x = extinctCache.x / divisor;
  extinctCache.y = extinctCache.y / divisor;
  extinctCache.z = extinctCache.z / divisor;
  meanExtinct = (extinctCache.x+extinctCache.y+extinctCache.z)/3;

  //force clipping in case of highly heterogenous media
  if(meanExtinct<0.0) {
    meanExtinct=0.01;
  }
  

  //could use for homogenous media
  //      stepSize = - ( log(drand48()) / ((static_cast<Participating *>(medium->surMat))->meanExtinct) );
  
  //using mean scatterring coefficient, reevaluate step size.
  stepSize = - ( log ( randVal) ) / (meanExtinct);
  step.normalize();
  step*=stepSize;

  //return out params
  location.x+=step.x;
  location.y+=step.y;
  location.z+=step.z;
  p.x = location.x;
  p.y = location.y;
  p.z = location.z;
  return step;
}

void Renderer::tracePhotonInParticipatingMedium(Photon &p, 
						std::shared_ptr<Surface> medium
						)
{
  //march participant through medium until reach boundary or scatter
  Point3Dd location(p.x, p.y, p.z);
  bool done=false;
  Point3Dd step;

  while((!done)&&USEFUL(p))
    {
      // determine step size
      step=estimateExtinctionCoefficient(medium,
					 static_cast<Participating*>(medium->surMat),
					 p,
					 location
					 );
      
      if(!medium->contains(location)) done=true;
      else {
	//still in medium. Check for scattering event
	if(storeVolumeDirectLight||p.bounced)
	  pVolMap->addPhoton(p);
	if(medium->nDoPartRoulette(p)==SCATTER) {
	  if(!p.bounced)
	    p.bounced=true;
	  //adjusts photon travel direction by phase function
	  medium->DoVolBRDF(p);
	} else {
	  //ABSORPTION:
	  done=true;
	  return;
	}
      }
    }
  
  if(USEFUL(p)) {
    //If we get here, the photon was emitted from the medium,
    //possibly after being scattered, and without being absorbed.
    //It also still has enough power that we care about using it
    //in our luminance estimates.
    //
    //We should reset its outgoing position to be just outside
    //the medium before continuing to trace it so that it will not
    //hit the medium again and so it will hit surfaces immediately
    //outside the medium that may be closer than the sample step we
    //were trying to take when we found ourselves outside the medium.
    Ray sampleRay(p.x-step.x,p.y-step.y,p.z-step.z,
		  step.x,step.y,step.z);
    sampleRay.dir.normalize();
    double tVal = medium->closestIntersect(sampleRay);
    Point4Dd leaving = sampleRay.GetPointAt(tVal+BUMPDISTANCE);
    
    leaving.dehomogenize();
    p.x = leaving.x; p.y=leaving.y; p.z=leaving.z;
    //we've passed through the medium.  Now trace the photon in the
    //				regular way.
    tracePhoton(p);
  }
}

//resets the incident direction of a photon after collision with a
//surface whose normal is specified here.
Photon& Renderer::resetIncidentDir(Photon &p,
				   Point3Dd& normal
				   )
{
  //The phong model's method of calculating a reflection ray for an incidet light source is used. This can be written as
  // reflectedDir = 2(DirToLight^ dot Normal^)*Normal^ - DirToLight^

  Point3Dd incidentDir(p.dx, p.dy, p.dz);
  Point3Dd dirToLight=incidentDir*-1;
  Point3Dd newDir = normal*2*(normal.dot(dirToLight)) - dirToLight;
  
  p.dx = newDir.x;
  p.dy = newDir.y;
  p.dz = newDir.z;

  return p;
}

/*
  void showMap(photonMap *)
  Takes a photonMap and renders it to the screen.  This involves:
  For all rays produced by the camera,
   Trace the ray to see if it intersects a surface.
   If it does, get incoming flux from photon Map.
 */
//Also does specular lighting
//for when photons should not be added to the global photon map
void Renderer::showMap(PhotonMap * map) {
  showMap(map,0,
	  myScene->getWindowWidth()*myScene->getWindowHeight()
	  );
}

void Renderer::showMap(PhotonMap * map, int start, int pixels)
{
  //  Ray * sampleRay;
  int width = myScene->getWindowWidth();
  int height = myScene->getWindowHeight();
  int totalRays = width * height;
  int lastRay=0; int mapSearchable = map->isSearchable();

  currentCamera=myScene->getCamera();
  if(!currentCamera) 
    {
      std::cerr << "No camera defined!";
      exit(1);
    }
  currentCamera->setSampleDims(width,height);
  int i,j;
  //  int i = start / width;
  //  int j = start % width;
  //  for(int i= start / width; i<top; i++)
  //    for(int j= start % width; j<right; j++)

      for(int place=start; place<start+pixels;place++)
      {
	i = place / width;
	j = place % width;

	Ray sampleRay = currentCamera->getRay(i,j);
	Point3Dd color(0,0,0);

	if(mapSearchable)
	  color=mapGetColor(&sampleRay,map);

#ifndef NOSPECULAR
	color+=getSpecularColor(&sampleRay);
#endif
	myScene->putPixel(j,
			  i,
			  color
			  );
      }
}

//gets specular light; currently does not extend to
//reflective/refractive media
Point3Dd Renderer::getSpecularColor(Ray * sampleRay)
{
  //First scan through all surfaces for the closest one
#ifdef DEBUG_BUILD
  static int total=0; static int intersected=0;
  total++;
#endif

  Point3Dd specPower(0,0,0);
  double tClose, tCurrent;
  tClose = 50000;
  auto itSurfaces = myScene->getSurfaces()->begin();  
  auto lastSurface = myScene->getSurfaces()->end();
  std::shared_ptr<Surface> closestSurface = NULL;
  while(itSurfaces!=lastSurface)
    { //for all surfaces in scene
      //determine the t-Value of the closest intersect
      tCurrent = (*itSurfaces)->closestIntersect
	(*sampleRay);
      
      if(
	 (tCurrent!=-1)
	 &&(tCurrent < tClose)
	 &&(currentCamera->InViewVol(sampleRay->GetPointAt(tCurrent)))
	 )
	{
	  tClose=tCurrent;
	  closestSurface = *itSurfaces;
	}
      itSurfaces++;
    }
  //Now the closest surface, if it exists, is in closestSurface

#ifdef DEBUG_BUILD
  if(!(total % 20000))
    std::cout << "Specular pass ratio of Hits/Misses: " << intersected << '/'
	 << total <<std::endl;
#endif;
  
  if(closestSurface&&(!closestSurface->participates()))
    {
#ifdef DEBUG_BUILD
      intersected++;
#endif
      Point3Dd specPowD;
      //calculate red, green, and blue specular highlighting.
      //move hitpoint out a little
      Point4Dd hp4 = sampleRay->GetPointAt(tClose-BUMPDISTANCE); 
      Point3Dd hitPoint(hp4.x,hp4.y,hp4.z);
      Point3Dd normal = closestSurface->getNormalAt(*sampleRay);
      auto lights = myScene->getLights();
      auto itLights = lights->begin();
      bool shadowed=false;
      for(;itLights!=lights->end();itLights++)
	{ //for each light in the scene
	  //get the ray back to the light
	  //iterate through surfaces to see if it's blocked.
	  double distToLight = (*itLights)->getDistance(hitPoint);
	  Ray LightRay = (*itLights)->getRayFromLightToward(hitPoint);
	  LightRay.dir.normalize();
	  Ray toLight = Ray(hp4.x,
			    hp4.y,
			    hp4.z,
			    -LightRay.dir.x,
			    -LightRay.dir.y,
			    -LightRay.dir.z
			    );
	  //FIXME: Is this used?
	  double tDist;
	  shadowed=false;
	  for(itSurfaces=myScene->getSurfaces()->begin();
	      itSurfaces!=myScene->getSurfaces()->end();
	      itSurfaces++)
	    {
	      if((tDist = (*itSurfaces)->closestIntersect(toLight))!=-1)
		{
		  //If the distance from the light to the current
		  //surface is less than the distance from the light
		  //to the surface we're seeing if is shadowed, the
		  //surface is shadowed relative to that light.
		  if( (toLight.GetPointAt(tDist)-toLight.src).norm() < distToLight )
		    {
		      shadowed=true;
		    }
		}
	    }

	  if(!shadowed)
	    {
	      Point3Dd lightDir(LightRay.dir.x,
				LightRay.dir.y,
				LightRay.dir.z);
	      Point3Dd sampleDir(sampleRay->dir.x,
				 sampleRay->dir.y,
				 sampleRay->dir.z
				 );
	      Point3Dd matSpec=closestSurface->DoSpecBRDF(lightDir,
							  sampleDir
							  );
	      double cosf;
	      if(g_specModel=HALFWAY) //we're using halfway model
		{
		  Ray h(hp4,((LightRay.dir*-1)+(sampleRay->dir*-1)));
		  Point3Dd hd(h.dir.x,h.dir.y,h.dir.z);

#ifdef DEBUG_BUILD
		  std::cout << "Light  = " << LightRay.dir*-1 <<std::endl;
		  std::cout << "v      = " << sampleRay->dir*-1 <<std::endl;
		  std::cout << "normal = " << normal <<std::endl;
		  std::cout << "halfway= " << hd <<std::endl;
		  std::cout << "hd.nrml= " << hd.dot(normal);
		  std::cout << "cosf   = " ;
		  std::cout << cosf <<std::endl;
		  std::cout << "---------\n";
#endif
		  cosf=hd.dot(normal)/(hd.norm()*normal.norm());

		}
	      if(cosf>0)
		{
		  cosf = pow(cosf,closestSurface->getSpecCo(g_specModel));
		  specPowD.x = cosf*(*itLights)->specular.x * matSpec.x;
		  specPowD.y = cosf*(*itLights)->specular.y * matSpec.y;
		  specPowD.z = cosf*(*itLights)->specular.z *
		    matSpec.z;
		  /*
		  std::cout << "Light power spec: " <<
		    (*itLights)->specular.x
		       << ',' << (*itLights)->specular.y
		       << ',' << (*itLights)->specular.z
		       <<std::endl;
		  std::cout << "Matspec: " << matSpec <<std::endl;
		  std::cout << "cosf: " << cosf <<std::endl;
		  std::cout << "specPowD = " << specPowD <<std::endl;
		  */
		  if(specPowD.x>0)
		    specPower.x+=specPowD.x;
		  if(specPowD.y>0)
		    specPower.y+=specPowD.y;
		  if(specPowD.z>0)
		    specPower.z+=specPowD.z;
		}
	      
	    }
	  #ifdef DEBUG_BUILD
	  else std::cout << "Shadowed\n";
	  #endif
	}
      //            std::cout << specPower <<std::endl;
      //      specPower = Point3Dd(1.0,0.0,0.0);
      //      specPower.x = 0.5;
    }
  return specPower;
}

std::shared_ptr<Surface> Renderer::closestSurfaceAlongRay(Ray * sampleRay, double &tClose) {
  double tCurrent;
  tClose = 50000;
  
  auto itSurfaces = myScene->getSurfaces()->begin();  
  auto lastSurface = myScene->getSurfaces()->end();
  std::shared_ptr<Surface> closestSurface = NULL;

#ifdef DEBUG_BUILD
  static int total=0; static int intersected=0;
  total++;
#endif
  
  while(itSurfaces!=lastSurface)
    { //for all surfaces in scene
      //determine the t-Value of the closest intersect
      tCurrent = (*itSurfaces)->closestIntersect
        (*sampleRay);
      
#ifdef DEBUG_BUILD
      if(!(total % 5000)) {
        std::cout << " Surface " << *itSurfaces 
                  << "tCurrent = " << tCurrent
                  << "; tClose = " << tClose
                  << "; InViewVol = " << currentCamera->InViewVol(sampleRay->GetPointAt(tCurrent))
                  <<std::endl;
      }
#endif      
      
      if(
         (tCurrent!=-1)
         &&(tCurrent < tClose)
         //	 &&(currentCamera->InViewVol(sampleRay->GetPointAt(tCurrent)))
         )
        {
          tClose=tCurrent;
          closestSurface = *itSurfaces;
        }
      itSurfaces++;
    }
  //Now the closest surface, if it exists, is in closestSurface

#ifdef DEBUG_BUILD
  if(!(total % 20000))
    std::cout << "Trace pass ratio of Hits/Misses: " << intersected << '/'
              << total <<std::endl;
  if(closestSurface) { intersected++; }
#endif

  return closestSurface;
}

Point3Dd Renderer::mapGetColor(Ray * sampleRay, PhotonMap * map)
{
  //If the photon map is too small, there is no illumination from it
  if(map->getSize() < MIN_MAP_SIZE) {
    return Point3Dd(0.0,0.0,0.0);
  }

  //First scan through all surfaces for the closest one.
  double tClose; //tClose is outparam
  std::shared_ptr<Surface> closestSurface = closestSurfaceAlongRay(sampleRay, tClose);
  
  if(closestSurface)
    { //we hit something

      //we need to ask the photon map for luminance information
      //move hitpoint out a little
      Point4Dd hp4 = sampleRay->GetPointAt(tClose-BUMPDISTANCE); 
      Point3Dd hitPoint(hp4.x,hp4.y,hp4.z);
      Point3Dd normal = closestSurface->getNormalAt(*sampleRay);
      Point3Dd flux;

#ifndef VOLMAP_ONLY	
      if(!closestSurface->participates()) {
        flux = map->getFluxAt(hitPoint,normal);
	if(!storeDirectLight) {
	  flux += directLightingLookingAlong(sampleRay);
	}
      } else 
#else
	if(closestSurface->participates())
#endif
	  //The ray from the eye enters a participating media.
	  //We want to compute the contribution to the radiance from
	  //direct illumination (single scattering) by ray-marching
	  //through the medium, at each point ray-marching back toward
	  //each light source in the scene.
	  {
	    hp4 = sampleRay->GetPointAt(tClose+BUMPDISTANCE);
	    hitPoint.x = hp4.x;
	    hitPoint.y = hp4.y;
	    hitPoint.z = hp4.z;
	    Point3Dd dir = Point3Dd(sampleRay->dir.x,sampleRay->dir.y,sampleRay->dir.z);
	    
	    flux=getIlluminationInMedium(hitPoint,dir,closestSurface,50);
	    //	    flux = pVolMap->getLuminanceAt(hitPoint);
	  }

      #ifdef DEBUG_BUILD
      std::cout << "image has pt with flux " << flux << std::endl;
      #endif
      return flux;
    }

  //we did not hit something
  return Point3Dd(0.0,0.0,0.0);
}

Point3Dd Renderer::getIlluminationInMedium(const Point3Dd &point, 
					   const Point3Dd &dir, 
					   const std::shared_ptr<Surface> surface,
					   const int marchsize) const {
  //calculate distance for which ray is in medium
  Ray sampleRay(point,dir);
  
  double tVal = surface->closestIntersect(sampleRay);

  //find the participating medium
  Participating * medium = (Participating *)surface->surMat;

  //split into marchsize segments
  double increment = tVal / (marchsize);
  //value of last staggerred (to prevent aliasing) sample point
  double lastRealVal = 0.0;
  //value of last unstaggerred sample point
  double lastVal = 0.0;
  //tVal of current point
  double curVal = 0.0;
  //real distance in 3-space world coords from last sample point
  double realDist = 0.0;

  //extinction coefficient cache
  Point3Dd extinctCo;

  //scattering coefficient cache
  Point3Dd scatCo;

  //multiplier; e^(-extinctCo*change in position), or 1 to start
  Point3Dd multiplier(1.0,1.0,1.0);

  //Total Scattering
  Point3Dd totalScat = Point3Dd(0.0,0.0,0.0);
  Point3Dd scat;

  for(int i=0; i<marchsize; i++) {
    //pick a random location on the next segment; this reduces
    //aliasing effects
    curVal=lastVal+(increment*drand48());
    realDist = (curVal - lastRealVal) * dir.norm();

    //we only need to use 1, while we are using homogenous media
    //FIXME

    //getIlluminationAtPointInMedium returns sum of all direct incident light adjusted by phase
    //function and for attenuation

    //get scattering coefficient at current location
    medium->copyScatCo(scatCo,point);

    //multiplier, if left over as non-1 from previous loops, accounts
    //for the sum of direct illumination that enters this segment
    //through its back
    Point3Dd directIllumination = storeVolumeDirectLight ? Point3Dd(0.0,0.0,0.0) : (getIlluminationAtPointInMedium(point, dir, surface, 1) * scatCo) ;
    scat = 
      ((
	//component from single scattering - direct computation via
	//ray marching
	directIllumination
	        +
       //component from multiple scattering - from volume photon map
	pVolMap->getLuminanceAt(point,dir,medium)
	) * (realDist)
       )
      *
      multiplier;

    totalScat+=scat;
    
    //prepare for next loop
    medium->copyExtinctCo(extinctCo,point);

     //update multiplier
     //account for product of all e^(-sig_t*delta(x))
    multiplier.x *= exp(-extinctCo.x*realDist);
    multiplier.y *= exp(-extinctCo.y*realDist);
    multiplier.z *= exp(-extinctCo.z*realDist);

     //next segment
     lastRealVal = curVal;
     lastVal+= increment;
  }
  
  //std::cerr << "totalScar " << totalScat <<std::endl;
  return totalScat;
}

//marchsize is the number of sample rays to use in integration to each
//light source in the scene for single scattering
Point3Dd Renderer::getIlluminationAtPointInMedium(const Point3Dd &point,
						  const Point3Dd &dir, 
						  const std::shared_ptr<Surface> surface,
						  const int marchsize) const {

  //common code
  auto lights = myScene->getLights();
  auto itLights = lights->begin();

  //find the participating medium
  Participating * medium = (Participating*)surface->surMat;

  Ray sampleRay;

  Point3Dd total(0.0,0.0,0.0);
  //to check boundary conditions
  Point3Dd temp;
  Point3Dd extinctCo;

  //single-scattering
  
  //The contribution from this point to the larger picture is
  // The distance moved through the medium, 
  // times the incident light in that dir from the light source, 
  // times the phase function, 
  // times sigma_s, the scatterring coefficient.

  //Integrate contribution from each light source
  //

  for(;itLights!=lights->end();itLights++) {
    //get a ray from the point to the light
    sampleRay = (*itLights)->getRayFromLightToward(point);
    
    //determine the distance inside the medium
    double tVal = surface->closestIntersect(sampleRay);
    
    //marchsize rays
    double increment = tVal / (double)marchsize;

    //actual length of rays
    double length = (sampleRay.dir*increment).norm();

    //extinction coefficient
    Point3Dd extinct;
    
    //homogenous media only
    //initial light, minus distance through medium, times extinction coefficient
    //times phase function from incident to current ray dir
    Point3Dd sourceDir(sampleRay.dir.x,sampleRay.dir.y,sampleRay.dir.z);
    sourceDir*=-1;

    //    temp=( (*itLights)->diffuse - (extinct * length * marchsize) ) * (medium->phaseFunction(sourceDir,dir));

    //for non-homogenous media
    //initial light, minus, for each step during ray marching,
    // dx * extinctCo(location)
    Point3Dd incomingLight = (*itLights)->diffuse;
    Point4Dd temp4d;

    for(int step=0; step<marchsize; step++) {
      temp4d = sampleRay.GetPointAt(tVal+increment*step);
      temp4d.dehomogenize();
      medium->copyExtinctCo(extinctCo,temp4d.x,temp4d.y,temp4d.z);
      Point3Dd extinctionAmounts = extinctCo*length;
      double extinctionSum = extinctionAmounts.x+extinctionAmounts.y+extinctionAmounts.z;
      extinctionSum = extinctionSum > 1.0 ? 1.0 : extinctionSum;
      /*
      std::cout << " eliminating " << extinctionSum << " of incident light at point " << point
		<< " and dir " << dir
		<< std::endl;
      */
      incomingLight *= (1.0-extinctionSum);
	//incomingLight -= ( extinctCo * length );
    }

    //Then multiply result by phase function for direction shift
    //    std::cerr << "pre-phase: " << incomingLight;
    //want phase function from the direction FROM the light (in) to the direction TOWARD the eye (out). soureDir is the direction from the light. dir*-1 is the direction back toward the eye.
    temp = incomingLight * medium->phaseFunction3D(sourceDir,dir*-1);
    //    std::cerr << " post-phase: " << temp <<std::endl;

    if(temp.x < 0) temp.x=0;
    if(temp.y < 0) temp.y=0;
    if(temp.z < 0) temp.z=0;

    total+=temp;
  }

  return total;
}






