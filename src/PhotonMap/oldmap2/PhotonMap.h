#ifndef PHOTONMAP_H_
#define PHOTONMAP_H_

#include <iostream.h>
#include <string>
#include "Photon/Photon.h"
#include "vector.h"
#include "list.h"


enum PHOTONMAP_STORAGE_TYPE { UNSORTED, KD_TREE };
enum KD_DIM { X, Y, Z, CHILD };
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

  void outputTree(int nRoot);

  std::ostream& out(std::ostream&);
  std::istream& in(std::istream&);

 protected:
  vector<Photon> unsortedPhotons;
  PHOTONMAP_STORAGE_TYPE storage;
  Photon ** kdTree;
  int kdSize;

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
  void medianRootSwapAndPartition(int nRoot, KD_DIM dim);
  void fillTree(int nRoot, list<Photon **> * pNodes);
  
};

#endif
