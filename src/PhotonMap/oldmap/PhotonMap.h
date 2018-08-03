#ifndef PHOTONMAP_H_
#define PHOTONMAP_H_

#include <iostream>
#include <queue>
#include <list>
#include <Photon/Photon.h>
#include <Point3Dd.h>
#include <PhotonPriorityQueue/PhotonPriorityQueue.h>

//amount to multiply photon power by
//#define POWERMULT 10000000000.0
#define POWERMULT 1.0
#define RANGEMULT 100000.0

//File formats for photon map storage
#define UNCOMPRESSED 0

//dimension specifiers for KD-Tree
#define DIM_X 0
#define DIM_Y 1
#define DIM_Z 2
#define DIM_CHILD 3

//Type of storage in photonarray
#define UNSORTED 0
#define QUICKSORT_X 4
#define KD_TREE 16

//max Number of neighbors to search for when computing flux
#define NUMNEIGHBORS 250

#define LIMMAXSEARCHRANGE 0.5

//define the square of the max search range for nearest neigbor searching
//#define MAXSEARCHRANGE 0.025

class PhotonMap
{
 public:

  PhotonMap();
  PhotonMap(PhotonMap&);
  ~PhotonMap();
  
  void initialize(int nSize);

  void addPhoton(Photon&);

  void buildTree();

  void outputTree(int);
  bool checkKDTree(int);

  void quickSort();

  void showMap();

  Point3Dd getFluxAt(Point3Dd &, Point3Dd& normal);

  Point3Dd getLuminanceAt(Point3Dd &);

  ostream& out(ostream&);
  istream& in(istream&);

  int getSize();

 protected:
  double distance(Photon &p, Point3Dd &loc);

  //kd-Tree functions:
  int kdBalance(int);
  void kdDimSort(int nMin, int nMax, int dim);
  void generateList(list <Photon *> * sList,
		    int rootNode,
		    int dim);
  void splitKDNodes(int splitNode,
		    int dim,
		    int rootNode,
		    vector<Photon *> * leftNodes,
		    vector<Photon *> * rightNodes
		    );
  int findMaxDim(int rootNode);
  int getHeightOf(int);
  void fillDimSpreads(int rootNode,
		      double &xMin,
		      double &xMax,
		      double &yMin,
		      double &yMax,
		      double &zMin,
		      double &zMax
		      );
  void fillTree(int nRoot, 
		vector<Photon *> * pNodes
		);
  void kdNN(int,
	    Point3Dd&,
	    PhotonPriorityQueue * 
	    //	    priority_queue<Photon *> *
	    );

  //quicksort functions
  void qSort(int nMin, int nMax);
  void qSortY(int nMin, int nMax);
  void qSortZ(int nMin, int nMax);
  
  Photon * photonArray;
  Photon ** kdTree;

  double maxdist;

  double MAXSEARCHRANGE;

  int lastPhoton;
  int kdSize;
  int nSize; //size of photonArray
  int storage; //the type of storage on the photons;
  //unsorted, quicksorted in dimension x, or kd-Tree
};

istream& operator>>(istream&,PhotonMap &);

#endif
