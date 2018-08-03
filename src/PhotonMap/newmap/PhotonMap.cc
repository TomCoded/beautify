#include "PhotonMap.h"

/* constructors and destructors */
PhotonMap::PhotonMap(){
  unsortedPhotons = new vector<Photon>();
}

PhotonMap::PhotonMap(PhotonMap& other){
}

PhotonMap::~PhotonMap(){
  delete unsortedPhotons();
}
/* end constructors and descructors */
/* build/query interface */

void PhotonMap::addPhoton(Photon &p){
  unsortedPhotons->push_back(p);
}

Point3Dd getFluxAt(Point3Dd &, Point3Dd& normal){
}

Point3Dd getLuminanceAt(Point3Dd &){
}

void PhotonMap::buildTree() {
  kdSize = unsortedPhotons.size();
  kdTree = (photon **) malloc(kdSize * sizeof(Photon *));
  for(int n=1; n<kdSize; n++) {
    kdTree[n]=&unsortedPhotons[n];
  }
  kdBalance(1);
  storage=KD_TREE;
}

/* private helper functions */
//recursively balances the kdTree
int PhotonMap::kdBalance(int nRoot) {
  if(!kdTree[nRoot]) return -1;
  if(nRoot * 2 < kdSize) {
    //has children
    //find splitting dimension
    KD_DIM dim=findMaxDim(nRoot);
    //find median; swap with root node
    //set flag to identify splitting dimension
    //need to partition set into nodes on either side of splitting
    //    plane
    ///... stuff
    //now load each set into the kdTree
    medianRootSwapAndPartition(nRoot,dim);

    //balance left and right children
    kdBalance(2*nRoot);
    if(nRoot*2+1<kdSize)
      kdBalance(2*nRoot+1);
  }
  else {
    kdTree[nRoot]->flag=DIM_CHILD;
  }
}

//creates ordered list of photons in dim to find median
//swaps median in dim with root if the two are different.
//swaps left and right nodes in tree of nRoot according to
//  location relative to median; orders tree
void medianRootSwapAndPartition(nRoot,KD_DIM dim) {
  List<Photon *> sortedPList();
  List<Photon *> leftList();
  List<Photon *> rightList();
  //generate sorted list in dimension dim of all photons below nRoot
  generateList(&sortedPList, nRoot, dim);
  List<Photon *>::iterator itList = sortedPList.begin();
  int listSize = sortedPList.size()/2;
  if(listSize) {
    for(int n=0;n<listSize;(itList++)) {
      n++;
      if(n==listSize) {
	leftList.push_back(*itList);
      }
    }
    List<Photon *>::iterator rightListStart = itList+1;
    //itList is now pointing at the median of the list; swap with root
    if(*itList!=kdTree[nRoot]) {
      //swap them if unequal
      Photon * temp=kdTree[nRoot];
      kdTree[nRoot] = *itList;
      *itList = kdTree[nRoot];
    }
    kdTree[nRoot]->flag=dim;
    int  n=0;
    for(itList=rightListStart; itList!=sortedPList.end(); itList++) {
      rightList.push_back(*itList);
    }
    //now load each set into the kdTree
    fillTree(nRoot*2,&leftNodes);
    fillTree(nRoot*2+1,&rightNodes);
}

//fills tree with photons in vector, in unspecified order
//removes them from the vector once they've been added.
void PhotonMap::fillTree(int nRoot, list<Photon *> * pNodes)
{
  if(!pNodes->empty())
    {
      if(nRoot<kdSize)
	{ //at least one node to be transferred, space for it exists.
	  kdTree[nRoot]=*pNodes->pop();
	  if(nRoot*2<kdSize) 
	    { //child nodes/trees exist
	      fillTree(nRoot*2,pNodes);
	      if(nRoot*2<kdSize+1)
	      fillTree(nRoot*2+1,pNodes);
	    }
	}
    }
  else
    {
      kdTree[nRoot]=0;
      if(2*nRoot<kdSize)
	{
	  fillTree(nRoot*2,pNodes);
	  if(2*nRoot+1<kdSize)
	    fillTree(nRoot*2+1,pNodes);
	}
    }
}


//FIXME: flags here set to be place in kdTree[] of current node
void generateList(&list<Photon *> sList, int nRoot, KD_DIM dim) {
  if(!kdTree[nRoot]) return;
  if(sList->empty()) {
    kdTree[nRoot]->flag=nRoot;
    sList->push_back(kdTree[nRoot]);
  }
  else {
    vector<Photon *>::iterator itList;
    double comp;
    switch(dim) {
    case DIM_X:
      comp=kdTree[nRoot]->x;
      //for loop finds first photon in list s.t. the photon is to the
      //  left of the rootNode in the x dimension
      for(itList=sList->begin();
	  (itList!=sList->end())
	    &&((*itList)->x < comp);
	  itList++);
      //now itList points to right place in list
      kdTree[nRoot]->flag=nRoot;
      sList->insert((--itList),kdTree[nRoot]);
      break;
    case DIM_Y:
      comp=kdTree[nRoot]->y;
      //for loop finds first photon in list s.t. the photon is to the
      //  left of the rootNode in the x dimension
      for(itList=sList->begin();
	  (itList!=sList->end())
	    &&((*itList)->y < comp);
	  itList++);
      //now itList points to right place in list
      kdTree[nRoot]->flag=nRoot;
      sList->insert((--itList),kdTree[nRoot]);
      break;
    case DIM_Z:
      comp=kdTree[nRoot]->z;
      //for loop finds first photon in list s.t. the photon is to the
      //  left of the rootNode in the x dimension
      for(itList=sList->begin();
	  (itList!=sList->end())
	    &&((*itList)->z < comp);
	  itList++);
      //now itList points to right place in list
      kdTree[nRoot]->flag=nRoot;
      sList->insert((--itList),kdTree[nRoot]);
      break;
    default:
      cerr << "PhotonMap given wrong dimension variable!\n";
      exit(1);
    }
  }
  if(nRoot*2 < kdSize) {//has children, balance them
    generateList(sList,nRoot*2,dim);
    if(nRoot*2+1 < kdSize) 
      generateList(sList,nRoot*2+1,dim);
  }
  kdTree[nRoot]->flag=nRoot;
}

//for a given portion of a kd-Tree, scan through all elts.
//locate the largest dimension (eg, photons maximum distance apart)
//and return it.  This reflects the splitting dim of the kd-tree
//rooted at nRoot.
// pre: kdTree[nRoot] and children are properly part of same subtree
KD_DIM findMaxDim(int nRoot) {
  double xMin, xMax, yMin, yMax, zMin, zMax;
  //set dimensions to be outside our allowed viewing area
  xMax=yMax=zMax=-SCOPE_MIN;
  xMin=yMin=zMin=SCOPE_MIN;
  //load proper values into dimensional barriers
  //ex: rightmost location of photon loaded into xMax
  fillDimSpreads(nRoot,
		 xMin,
		 xMax,
		 yMin,
		 yMax,
		 zMin,
		 zMax
		 );
  //compute differences
  double yDiff = yMax - yMin;
  double xDiff = xMax - xMin;
  double zDiff = zMax - zMin;
  //return largest dimension
  return ( yDiff > xDiff ? ( zDiff > yDiff ? Z : Y ) : (zDiff > xDiff ? Z : X) );
}

//used by findMaxDim
//scans kdTree starting at rootNode and puts smallest/largest elements
//in respective parameters, passed by reference.
void PhotonMap::fillDimSpreads(int rootNode,
			       double &xMin,
			       double &xMax,
			       double &yMin,
			       double &yMax,
			       double &zMin,
			       double &zMax
			       )
{
  if(kdTree[rootNode])
  {
    if(kdTree[rootNode]->x>xMax)
      xMax=kdTree[rootNode]->x;
    if(kdTree[rootNode]->x<xMin)
      xMin=kdTree[rootNode]->x;
    if(kdTree[rootNode]->y>yMax)
      yMax=kdTree[rootNode]->y;
    if(kdTree[rootNode]->y<yMin)
      yMin=kdTree[rootNode]->y;
    if(kdTree[rootNode]->z>zMax)
      zMax=kdTree[rootNode]->z;
    if(kdTree[rootNode]->z<zMin)
      zMin=kdTree[rootNode]->z;
    if(2*rootNode+1<kdSize) 
      { //process child nodes
	fillDimSpreads(2*rootNode,
		       xMin,
		       xMax,
		       yMin,
		       yMax,
		       zMin,
		       zMax);
	fillDimSpreads(2*rootNode+1,
		       xMin,
		       xMax,
		       yMin,
		     yMax,
		       zMin,
		       zMax);
      }
  }
}

/* input/output interface */
ostream& PhotonMap::out(std::ostream&){
  int FileType = UNCOMPRESSED_V2;
  switch(FileType)
    {
      case UNCOMPRESSED_V2;
      o << "BEGIN_PHOTONMAP ";
      o << FileType;
      o << ' ';
      o << " END_PHOTONMAP";
      break;
    default:
      cerr << "unknown file type\n";
      break;
    }
  return o;
}

istream& in(std::istream&){
  string beginString; int readFileType;
  char c;
  is >> beginString;
  if(beginString!="BEGIN_PHOTONMAP")
    {
      cerr << "Error reading photon map file.  No \"BEGIN_PHOTONMAP\""
	   << " string found\n";
    }
  else
    {
      is >> readFileType;
      switch(readFileType)
	{
	case UNCOMPRESSED:
	  cerr << "File is of antiquated type\n";
	  break;
	case UNCOMPRESSED_V2:
	  cerr << "Function not yet implemented\n";
	  break;
	}
    }
}
