#ifndef PHOTONMAP_H_
#define PHOTONMAP_H_

#include "Photon.h"

enum PHOTONMAP_STORAGE_TYPE { UNSORTED, KD_TREE };
enum KD_DIM { X, Y, Z, CHILD };

class PhotonMap {
 public:
  PhotonMap();
  PhotonMap(PhotonMap&);
  ~PhotonMap();

  void addPhoton(Photon&);

  void buildTree();

  Point3Dd getFluxAt(Point3Dd &, Point3Dd& normal);

  Point3Dd getLuminanceAt(Point3Dd &);

  ostream& out(ostream&);
  istream& in(istream&);

 protected:
  vector<Photon> unsortedPhotons;
  PHOTONMAP_STORAGE_TYPE storage;
  Photon ** kdTree;
  int kdSize;

  void generateList(&vector<Photon *>, int nRoot, KD_DIM dim);

};

#endif
