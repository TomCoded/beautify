#ifndef PHOTONMAP_H_
#define PHOTONMAP_H_

#include <parallel_pm.h>
#include <iostream.h>
#include <string>
#include "Photon/Photon.h"
#include "PhotonPriorityQueue/PhotonPriorityQueue.h"
#include "vector.h"
#include "list.h"

#ifdef PI
#undef PI
#endif

#define PI 3.1415926535897932384626433832795

//If a photon map is smaller than this size,
//we consider it to have no valuable lighting info
#define MIN_MAP_SIZE 10

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

  Point3Dd getLuminanceAt(Point3Dd &);

  int getSize();

  //set number of neighbors to use for radiance estimates.
  void setNumNeighbors(int numNeighbors);

  //We will always check photons closer to location
  //than sqrt(minSearchSqr) when generating
  //radiance estimates.
  void setMinSearch(double minSearchSqr);

  void outputTree(int nRoot);

  //parallel stuff

  void getArrMembers(Photon *mule);

#ifdef PARALLEL
  void distributeTree();
#endif

  ostream& out(ostream&);
  istream& in(istream&);

 protected:
  vector<Photon> unsortedPhotons;
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
  double inline distance(Photon *p, Point3Dd& l);
  //updates passed in-out queue with nearest neighbors, recursively
  void findNearestNeighbors(int phLoc, Point3Dd &loc,PhotonPriorityQueue *Q);
  
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
