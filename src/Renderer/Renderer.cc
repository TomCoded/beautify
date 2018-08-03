//Renderer.cc
//(C) 2002 Tom White

#include <parallel_pm.h>
#include <allIncludes.h>
#include <PhotonMap/PhotonMap.h>

//#define DEBUG_BUILD

extern int antialias;
extern int g_specModel;

//default constructor
Renderer::Renderer():
  myScene(0), ambient(0.5,0.5,0.5), currentCamera(0), pMap(0), 
  seed(21), pVolMap(0)
  //21 decent seed (based on spotty visual evidence)
{}

//copy constructor
Renderer::Renderer(Renderer& other)
{
  myScene = other.myScene;
  currentCamera = other.currentCamera;
  ambient = other.ambient;
  otherSurfaces = other.otherSurfaces;
  pMap = other.pMap;
  seed=other.seed;
  pVolMap=other.pVolMap;
}

Renderer::Renderer(Scene * myScene):
  ambient(0.5,0.5,0.5), currentCamera(0), pMap(0),
  seed(21), pVolMap(0)
{
  setScene(myScene);
}

void Renderer::setScene(Scene * myScene)
{
  this->myScene = myScene;
  ambient = myScene->ambient;
  currentCamera = myScene->getCamera();
}

void Renderer::setSeed(int s)
{
  seed=s;
}

//destructor
Renderer::~Renderer()
{
  if(pMap) delete pMap;
  if(pVolMap) delete pVolMap;
}

//Management functions

//map() calls the photon mapper with default nPhotons
PhotonMap * Renderer::map()
{
  map(DEFAULT_PHOTONS);
}

PhotonMap * Renderer::getVolMap()
{
  return pVolMap;
}

//map() runs the photon mapper
//nPhotons is the number of photons to create from lights, total.
PhotonMap * Renderer::map(int nPhotons)
{

#ifdef PARALLEL
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
#endif

  if(pMap) {
    delete pMap;
  }
  if(pVolMap)
    delete pVolMap;
  pVolMap = new PhotonMap();
  pMap = new PhotonMap();
  

  //cout << "Initializing Photon Maps...\n";
  //  pVolMap->initialize(nPhotons);
  //  pMap->initialize(MAPMULT*nPhotons);
  vector<Light *> * lights = myScene->getLights();
  vector<Light *>::iterator itLights = lights->begin();
  int nLights = lights->size();
  int nPhotonsForThisLight=0;
  Point3Dd totalPower(0,0,0);
  for(;itLights!=lights->end(); itLights++)
    { //determine total power of lights in the scene
      totalPower+=(*itLights)->getPower();
    }

  double dAvgTotalPower = (totalPower.x+totalPower.y+totalPower.z)/3;
  double dAvgPower=0;
  //seed with non-random value to prevent massive variation in
  //re-rendering of same image over time.
  for(itLights=lights->begin();
      itLights!=lights->end();
      itLights++)
    { //for each light in the scene
      //calculate the number of photons needed
      //don't need totalPower anymore
      srand48(seed);

#ifdef DEBUG_BUILD
#ifdef PARALLEL
      cout << rank << ':';
#endif
      //      cout << "seed is " << seed << endl;
#endif
      totalPower=((*itLights)->getPower());
      dAvgPower=(totalPower.x+totalPower.y+totalPower.z)/3;
      nPhotonsForThisLight=nPhotons*(dAvgPower/dAvgTotalPower);
#ifdef PARALLEL
      if(g_parallel) {
	int nodes;
	MPI_Comm_size(MPI_COMM_WORLD, &nodes);
	Point3Dd pow = (*itLights)->getPower();
	pow.x /= nodes;
	pow.y /= nodes;
	pow.z /= nodes;
	(*itLights)->setPower(pow);
      }
#endif

      (*itLights)->addPhotonsToMap(nPhotonsForThisLight,
				   pMap,
				   this
				   );
#ifdef PARALLEL
      if(g_parallel) {
	int nodes;
	MPI_Comm_size(MPI_COMM_WORLD, &nodes);
	Point3Dd pow = (*itLights)->getPower();
	pow.x *= nodes;
	pow.y *= nodes;
	pow.z *= nodes;
	(*itLights)->setPower(pow);
      }
#endif
    }

  return pMap;
}

//run() ran the ray tracer
int Renderer::run()
{}

Point3Dd Renderer::getColor(Ray * sampleRay)
{
  return getColor(sampleRay,myScene->getSurfaces());
}

Point3Dd Renderer::getColor(
			    Ray * sampleRay,
			    vector<Surface *> * surfaces
			    )
{
  vector<Surface *>::iterator itSurf;
  vector<Surface *> intersectedSurfaces;
  vector<Surface *>::iterator itIntSurf;
  Point3Dd color(0,0,0);
  Surface * closestSurface = 0;
  Surface * farthestSurface = 0;
  double tClose = 100000000; double tLast = tClose; double tFar=0;
  if(recursionDepth>1) return Point3Dd(0,0,0);
  else recursionDepth++;
  for(itSurf = surfaces->begin(); itSurf != surfaces->end(); itSurf++)
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
	{ //add to list of intersected surfaces
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
	//we need to set up Surface * otherSurfaces in case
	//the surface uses a reflective shader that needs to
	//know the other surfaces to test the ray
	//against
	itIntSurf=intersectedSurfaces.begin();
	otherSurfaces = vector<Surface *>();
	for(++itIntSurf;
	    itIntSurf!=intersectedSurfaces.end();
	    itIntSurf++)
	  otherSurfaces.push_back(*itIntSurf);
	Point4Dd hp4 = sampleRay->GetPointAt(tClose);
	Hit hit(Point3Dd(hp4.x,hp4.y,hp4.z),
		closestSurface,
		normal,
		Ray(*sampleRay),
		tClose
		);
	color=
	  (closestSurface->surShader->getColor(hit));
      } else {
	//participating medium.
      }
    }
  recursionDepth--;
  return color;
}

//I/O functions
istream& Renderer::in(istream& in)
{
  UNIMPLEMENTED("Renderer.in");
}

ostream& Renderer::out(ostream& out)
{
  UNIMPLEMENTED("Renderer.out")
}

vector<Light *> * Renderer::getAllLights()
{
  return myScene->getLights();
}

vector<Surface *> * Renderer::getOtherSurfaces()
{
  if(otherSurfaces.size())
    return &otherSurfaces;
  else return 0;
}


//gets lights in the scene, that are visible to a certain point.
//the intensity of the light source decreases linearly 
//with the transparency of the surfaces through which
//the light ray passes.
vector<Light *> * Renderer::getApparentLights(Point3Dd point)
{
  vector<Light *>::iterator itLights;
  vector<Light *>::iterator itLastLight;
  vector<Surface *>::iterator itSurfaces;
  vector<Light *> * lightsToReturn = new vector<Light *>();
  double tLast, tClose, tPoint;
  double transparency=1;
  Point4Dd raySrc, rayDir; 
  Ray localSampleRay; Ray sampleRay;

  itLights = myScene->getLights()->begin();
  itSurfaces = myScene->getSurfaces()->begin();
  itLastLight = myScene->getLights()->end();
  Surface * lastSurface = *(myScene->getSurfaces()->end());

  for(;itLights != itLastLight; itLights++)
    { //for each light in the scene
      //calculate t-point of current point
      sampleRay = Ray((*itLights)->getRayTo(point));
      tPoint = sampleRay.TofClosestPoint(Point4Dd(point,0));
      tClose = -1;
      for(int done=false;(!done)&&((*itSurfaces) != lastSurface); itSurfaces++)
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
      if((tClose==-1)&&(transparency!=0)) //the light is visible
	lightsToReturn->push_back((**itLights).transparent(transparency));
    }
  return lightsToReturn;
}

void Renderer::deAllocate(vector <Light *> * lights)
{
  vector<Light *>::iterator itDestroyer = lights->begin();
  while( itDestroyer != lights->end() )
    { //destroy all light sources in the vector
      if(*itDestroyer) delete (*itDestroyer);
      itDestroyer++;
    }
  delete lights;
}

vector<Light *> * Renderer::getVisibleLights(Point3Dd point)
{
  vector<Light *>::iterator itLights;
  vector<Light *>::iterator itLastLight;
  vector<Surface *>::iterator itSurfaces;
  vector<Light *> * lightsToReturn = new vector<Light *>();
  itLights = myScene->getLights()->begin();
  itSurfaces = myScene->getSurfaces()->begin();
  itLastLight = myScene->getLights()->end();
  Surface * lastSurface = *(myScene->getSurfaces()->end());
  Point4Dd raySrc, rayDir; 
  double tLast, tClose, tPoint;
  Ray localSampleRay; Ray sampleRay;
  for(;itLights != itLastLight; itLights++)
    { //for each light in the scene
      //calculate t-point of current point
      sampleRay = Ray((*itLights)->getRayTo(point));
      tPoint = sampleRay.TofClosestPoint(Point4Dd(point,0));
      tClose = -1;
      for(int done=false;(!done)&&(*itSurfaces)!=lastSurface; itSurfaces++)
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
Photon& Renderer::tracePhoton(Photon &p)
{
#ifdef DEBUG_BUILD
  static int total=0; static int intersected=0;
  total++;
#endif
  //First scan through all surfaces for the closest one.
  double tClose, tCurrent;
  tClose = 1000000;
  vector<Surface *>::iterator itSurfaces;
  itSurfaces = myScene->getSurfaces()->begin();  
  Surface * lastSurface = *(myScene->getSurfaces()->end());
  Surface * closestSurface = NULL;
  Ray * sampleRay = new Ray(p.x,
			    p.y,
			    p.z,
			    p.dx,
			    p.dy,
			    p.dz
			    );
  Point3Dd location(p.x,p.y,p.z);
  while((*itSurfaces)!=lastSurface)
    { //for all surfaces in scene
      //determine the t-Value of the closest intersect
      tCurrent = (*itSurfaces)->closestIntersect(*sampleRay);
      //      cout << "Implicit value of " << location << " is " <<
      //	(*itSurfaces)->implicit(location) << endl;
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
	cout << "Current ratio of Hits/Misses: " << intersected << '/'
	     << total << endl;
#endif

  if(closestSurface)
    { //we hit something
#ifdef DEBUG_BUILD
      intersected++;
#endif
#ifdef DEBUG_BUILD
      cout << "tClose = " << tClose << endl;
      if (tClose < 0) 
	cout << "NEGTCLOSE\n";
      cout << "point at tClose = " << sampleRay->GetPointAt(tClose) <<
	endl;
      cout << "BUMPDISTANCE = " << BUMPDISTANCE << endl;
      cout << "point at tClose - BUMPDISTANCE = " <<
	sampleRay->GetPointAt(tClose-BUMPDISTANCE) << endl;
      cout << "point at tClose + BUMPDISTANCE = " <<
	sampleRay->GetPointAt(tClose+BUMPDISTANCE) << endl;
      cout << "SampleRay = " << *sampleRay << endl;
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
	      Photon p2 = p;
	      if((p2.r > 0.0)
		 ||(p2.g>0.0)
		 ||(p2.b>0.0)
		 )
		{
		  p2.x=iPoint.x;
		  p2.y=iPoint.y;
		  p2.z=iPoint.z;
		  //just using lambertian model for now.
		  //		  cout << "pre -BRDF p2 is " << p2 << endl;
		  //		  closestSurface->DoBRDF(p2);
		  //		  cout << "post-BRDF p2 is " << p2 << endl;
		  p=p2;
		  pMap->addPhoton(p2);
		}
	      resetIncidentDir(p,nPoint);
	      //want to adjust each color channel to take into account
	      //the probability of the photon surviving.
	      //first, de-allocate sample ray.
	      delete sampleRay;
	      return tracePhoton(p);
	      break;
	    case SPECULAR_REFLECTION:
	      NULL_PHOTON;
	      delete sampleRay;
	      return p;
	      break;
	    case ABSORPTION:
	      NULL_PHOTON;
	      delete sampleRay;
	      return p;
	      break;
	    default: 
	      cerr << "Error: unknown Russian Roulette result in Renderer.cc\n";
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
#if 0
	  cout << "Outer: ";
	  cout << "Photon: " << p << ':';
#endif
	  switch(closestSurface->nDoPartRoulette(p))
	    {
	    case SPECULAR_REFLECTION:
	    case DIFFUSE_REFLECTION:
	      cerr << "Called non-volumetric nRoulette() on ";
	      cerr << closestSurface << endl;
	      exit(1);
	    case SCATTER:
	      //photon is scattered here; store it in the volume
	      //photon map
	      closestSurface->DoVolBRDF(p);
	      pVolMap->addPhoton(p);
	      break;
	    case ABSORPTION:
	      NULL_PHOTON;
	      return p;
	      break;
	    case STEP:
	      //photon not scattered here; march on.
	      participantMarch(p,closestSurface);
	      NULL_PHOTON;
	      return p;
	      break;
	    default:
	      cerr << "Error: unknown volumetric Russian Roulette result in Renderer.cc\n";
	      exit(1);
	    }
	}
    }
  delete sampleRay;
  NULL_PHOTON;
  return p;
}
#undef NULL_PHOTON

void Renderer::participantMarch(Photon &p, 
				Surface * medium
				)
{
  //march participant through medium until reach boundary or scatter
  Point3Dd location(p.x, p.y, p.z);
  bool done=false;
  //do uniform downscale of photon incident direction to make this ok
  //  for non massive objects.
  //  cout << "---\n";
  p.dx = p.dx / 10.0;
  p.dy = p.dy / 10.0;
  p.dz = p.dz / 10.0;
#if 1
  static Point3Dd step(0.0,0.0,0.0);
  static int count=0;
  count++;
  step.x += p.dx;
  step.y += p.dy;
  step.z += p.dz;
  if(!(count%10000)) {
    cout << "Average step size: " << step.norm()/(double)count << endl;
  }
#endif
  while(!done)
    {
      //      cout << "Stepping " << p.dx << ',' << p.dy << ',' <<
      //      p.dz << endl;
      //      cout << "Now at " << p.x << ',' << p.y << ',' << p.z << endl;
      location.x+=p.dx;
      location.y+=p.dy;
      location.z+=p.dz;
      p.x = location.x;
      p.y = location.y;
      p.z = location.z;
      if(!medium->contains(location)) done=true;
      else
	{ //still in medium check for scattering event
#if 0
	  cout << "Inner: ";
	  cout << "Photon: " << p << ':';
#endif
	  switch(medium->nDoPartRoulette(p))
	    {
	    case SCATTER:
	      //photon is scattered here; store it in the volume
	      //photon map.  It does not leave the medium.
	      medium->DoVolBRDF(p);
	      if(p.r||p.g||p.b)
		pVolMap->addPhoton(p);
	      done=true;
	      p.r=p.g=p.b=0.0;
	      break;
	    case ABSORPTION:
	      p.r=0;
	      p.g=0;
	      p.b=0;
	      done=true;
	      break;
	    case STEP:
	      //photon not scattered here; march on.
	      medium->DoVolBRDF(p);
	      break;
	    default:
	      cerr << "Error: unknown volumetric Russian Roulette result in Renderer.cc\n";
	      exit(1);
	    }
	}
    }
  //we've passed through the medium.  Now trace the photon in the
  //				regular way.
  if(p.r||p.g||p.b)
    tracePhoton(p);
}

//resets the incident direction of a photon after collision with a
//surface whose normal is specified here.
Photon& Renderer::resetIncidentDir(Photon &p,
				   Point3Dd& normal
				   )
{
  //The phong model;  Calculate reflection ray, opposite incident ray
  //about normal.
  //  cout << "Before reseting incident direction: " << p << endl;
  //  cout << "Resetting incident direction about " << normal << endl;
  //reflected vector = incidentDir + 2 * (-incidentDir dot normal) /
  //  (normal.norm squared) * normal
  Point3Dd incidentDir(p.dx, p.dy, p.dz);
  //  normal=normal.normalize();
  double LdN2 = 2*(normal.dot(incidentDir*-1));
  Point3Dd rPlusLid = (normal*(LdN2/((normal.norm()*normal.norm()))));
  Point3Dd newDir((incidentDir)+rPlusLid);
  p.dx = newDir.x;
  p.dy = newDir.y;
  p.dz = newDir.z;
  //  cout << "After  reseting incident direction: " << p << endl;
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
  int lastRay=0;
  currentCamera=myScene->getCamera();
  if(!currentCamera) 
    {
      cerr << "No camera defined!";
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
 	Point3Dd color=mapGetColor(&sampleRay,map);
	color+=getSpecularColor(&sampleRay);
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
  vector<Surface *>::iterator itSurfaces;
  itSurfaces = myScene->getSurfaces()->begin();  
  Surface * lastSurface = *(myScene->getSurfaces()->end());
  Surface * closestSurface = NULL;
  while((*itSurfaces)!=lastSurface)
    { //for all surfaces in scene
      //determine the t-Value of the closest intersect
      tCurrent = (*itSurfaces)->closestIntersect
	(*sampleRay);
      if((tCurrent!=-1)&&(tCurrent < tClose))
	{
	  tClose=tCurrent;
	  closestSurface = *itSurfaces;
	}
      itSurfaces++;
    }
  //Now the closest surface, if it exists, is in closestSurface

#ifdef DEBUG_BUILD
  if(!(total % 20000))
    cout << "Specular pass ratio of Hits/Misses: " << intersected << '/'
	 << total << endl;
#endif;
  
  if(closestSurface)
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
      vector<Light *> * lights = myScene->getLights();
      vector<Light *>::iterator itLights = lights->begin();
      bool shadowed=false;
      for(;itLights!=lights->end();itLights++)
	{ //for each light in the scene
	  //get the ray back to the light
	  //iterate through surfaces to see if it's blocked.
	  Ray LightRay = (*itLights)->getRayTo(hitPoint);
	  LightRay.dir.normalize();
	  Ray toLight = Ray(hp4.x,
			    hp4.y,
			    hp4.z,
			    -LightRay.dir.x,
			    -LightRay.dir.y,
			    -LightRay.dir.z
			    );
	  double tDist;
	  shadowed=false;
	  for(itSurfaces=myScene->getSurfaces()->begin();
	      itSurfaces!=myScene->getSurfaces()->end();
	      itSurfaces++)
	    {
	      if((*itSurfaces)->closestIntersect(toLight)!=-1)
		{
		  if((*itSurfaces)!=closestSurface)
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
		  /*
		  cout << "Light  = " << LightRay.dir*-1 << endl;
		  cout << "v      = " << sampleRay->dir*-1 << endl;
		  cout << "normal = " << normal << endl;
		  cout << "halfway= " << hd << endl;
		  cout << "hd.nrml= " << hd.dot(normal);
		  cout << "cosf   = " << endl;
		  cout << "---------\n";
		  if(cosf > 0.3) cout << "cosf = " << cosf << endl;
		  */
		  //		  hd.dot(normal)/(hd.norm()*normal.norm()) << endl;
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
		  cout << "Light power spec: " <<
		    (*itLights)->specular.x
		       << ',' << (*itLights)->specular.y
		       << ',' << (*itLights)->specular.z
		       << endl;
		  cout << "Matspec: " << matSpec << endl;
		  cout << "cosf: " << cosf << endl;
		  cout << "specPowD = " << specPowD << endl;
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
	  else cout << "Shadowed\n";
	  #endif
	}
      //            cout << specPower << endl;
      //      specPower = Point3Dd(1.0,0.0,0.0);
      //      specPower.x = 0.5;
    }
  return specPower;
}

//note: deletes the ray that it is passed
Point3Dd Renderer::mapGetColor(Ray * sampleRay, PhotonMap * map)
{
  //If the photon map is too small, there is no illumination from it
  if(map->getSize() < MIN_MAP_SIZE) {
    delete sampleRay;
    return Point3Dd(0.0,0.0,0.0);
  }

  //First scan through all surfaces for the closest one.

#ifdef DEBUG_BUILD
  static int total=0; static int intersected=0;
  total++;
#endif

  double tClose, tCurrent;
  tClose = 50000;
  vector<Surface *>::iterator itSurfaces;
  itSurfaces = myScene->getSurfaces()->begin();  
  Surface * lastSurface = *(myScene->getSurfaces()->end());
  Surface * closestSurface = NULL;

  while((*itSurfaces)!=lastSurface)
    { //for all surfaces in scene
      //determine the t-Value of the closest intersect
      tCurrent = (*itSurfaces)->closestIntersect
	(*sampleRay);
      if((tCurrent!=-1)&&(tCurrent < tClose))
	{
	  tClose=tCurrent;
	  closestSurface = *itSurfaces;
	}
      itSurfaces++;
    }
  //Now the closest surface, if it exists, is in closestSurface

#ifdef DEBUG_BUILD
      if(!(total % 20000))
	cout << "Trace pass ratio of Hits/Misses: " << intersected << '/'
	     << total << endl;
#endif

  if(closestSurface)
    { //we hit something

#ifdef DEBUG_BUILD
      intersected++;
#endif

  //we need to ask the photon map for luminance information
      //move hitpoint out a little
      Point4Dd hp4 = sampleRay->GetPointAt(tClose-BUMPDISTANCE); 
      Point3Dd hitPoint(hp4.x,hp4.y,hp4.z);
      Point3Dd normal = closestSurface->getNormalAt(*sampleRay);
      Point3Dd flux;

      if(!closestSurface->participates())
	flux = map->getFluxAt(hitPoint,normal);
      else 
	{
	  hp4 = sampleRay->GetPointAt(tClose+BUMPDISTANCE);
	  hitPoint.x = hp4.x;
	  hitPoint.y = hp4.y;
	  hitPoint.z = hp4.z;
	  flux = pVolMap->getLuminanceAt(hitPoint);
	}
      return flux;
    }
  return Point3Dd(0.0,0.0,0.0);
}








