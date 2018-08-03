#ifndef PHOTONMAP_H_
#define PHOTONMAP_H_

#include <parallel_pm.h>
#include <iostream>
#include <string>
#include "Photon/Photon.h"
#include "PhotonPriorityQueue/PhotonPriorityQueue.h"
#include "Material/Participating/Participating.h"
#include <vector>
#include <list>

#ifdef PI
#undef PI
#endif

#define PI 3.1415926535897932384626433832795

//If a photon map is smaller than this size,
//we consider it to have no valuable lighting info
#define MIN_MAP_SIZE 10

//If a Photon has negligible power (Less than P_THRESH when summed
//over all color channels) We will not add it.
#define P_THRESH 1e-40

extern Point3Dd g_photonPowerLow;

enum PHOTONMAP_STORAGE_TYPE { UNSORTED, KD_TREE };
enum KD_DIM { X, Y, Z, CHILD, NULL_PHOTON, NOT_SET };
enum FILE_TYPE { UNCOMPRESSED=0, UNCOMPRESSED_V2 };

#define SCOPE_MIN -10000
#define SCOPE_MAX 10000

class PhotonMap {
 public:
  PhotonMap();
  PhotonMap(PhotonMap&);
  ~PhotonMap();

  void addPhoton(Photon&);

  void buildTree();

  Point3Dd getFluxAt(Point3Dd &, Point3Dd& normal);

  Point3Dd getLuminanceAt(const Point3Dd &loc, 
			  const Point3Dd& dir, 
			  Participating *medium);

  //returns number of photons in the map
  int getSize() const;
  
  //returns true if the map can be searched for
  //flux or luminance estimates.
  bool isSearchable() const;

  //set number of neighbors to use for radiance estimates.
  void setNumNeighbors(const int numNeighbors);

  //We will always check photons closer to location
  //than std::sqrt(minSearchSqr) when generating
  //radiance estimates.
  void setMinSearch(const double minSearchSqr);

  //Any photons further away than this will be discarded
  void setMaxDist(const double maxDist);

  void outputTree(int nRoot);

  //parallel stuff

  void getArrMembers(Photon *mule);

#ifdef PARALLEL
  //distributes copy of KD-Tree to each child
  void distributeTree();
  //gathers all unsorted photons from children
  void gatherPhotons(int maxPhotons);
#endif

  std::ostream& out(std::ostream&);
  std::istream& in(std::istream&);

 protected:
  std::vector<Photon> unsortedPhotons;
  PHOTONMAP_STORAGE_TYPE storage;
  Photon ** kdTree;
  int kdSize;
  double minSearchSqr;

  //number of neighbors to use at most in nearest neighbor search
  int numNeighbors;


  //maximum range to search for photons
  double maxDist;
  //square of that
  double maxDistSqr;
  //the prior two vars are seeded from this on each new search
  double maxDistDefault;

  //luminance/flux helper functions
  //returns squared distance between photon and point
  double inline distance(const Photon *p, const Point3Dd& l);
  //updates passed in-out queue with nearest neighbors, recursively
  void findNearestNeighbors(int phLoc, const Point3Dd &loc,PhotonPriorityQueue *Q);
  
  /* kdBalance and helper functions, for building the kdTree */
  int kdBalance(int nRoot);
  KD_DIM findMaxDim(int nRoot);
  void fillDimSpreads(int rootNode,
		      double &xMin,
		      double &xMax,
		      double &yMin,
		      double &yMax,
		      double &zMin,
		      double &zMax
		      );
  void generateList(list<Photon **> * sList, int nRoot, KD_DIM dim);
  void medianRootSwapAndPartition(int nRoot, KD_DIM dim,
		list<Photon *> *leftList, list<Photon *> *rightList);
  void fillTree(int nRoot, list<Photon *> * pNodes);
  
};

#endif
