#include "PhotonMap.h"

/* constructors and destructors */
PhotonMap::PhotonMap(){
}

PhotonMap::PhotonMap(PhotonMap& other){
}

PhotonMap::~PhotonMap(){
}
/* end constructors and descructors */
/* build/query interface */

void PhotonMap::addPhoton(Photon &p){
  unsortedPhotons.push_back(p);
}

Point3Dd PhotonMap::getFluxAt(Point3Dd &, Point3Dd& normal){
}

Point3Dd PhotonMap::getLuminanceAt(Point3Dd &){
}

int PhotonMap::getSize() {
  return unsortedPhotons.size();
}

void PhotonMap::buildTree() {
  kdSize = unsortedPhotons.size()+1;
  kdTree = (Photon **) malloc(kdSize * sizeof(Photon *));
  for(int n=1; n<kdSize; n++) {
    kdTree[n]=&unsortedPhotons[n-1];
    std::cout << "Assigning " << &unsortedPhotons[n-1] << " to kdTree[" <<
      n << "]\n";
  }
  kdBalance(1);
  storage=KD_TREE;
}

/* private helper functions */
//recursively balances the kdTree
int PhotonMap::kdBalance(int nRoot) {
  std::cout << "kdBalance(" << nRoot << ")\n";
  std::cout << "kdSize = " << kdSize <<std::endl;
  std::cout << "kdTree[nRoot] = " << kdTree[nRoot] <<std::endl;
  if(!kdTree[nRoot]) return -1;
  std::cout << "**kdTree[2,4]=" << kdTree[2] << ',' << kdTree[4] <<std::endl;
  if(nRoot * 2 < kdSize) {
    std::cout << "inside kdBal() loop\n";
    //has children
    //find splitting dimension
    KD_DIM dim=findMaxDim(nRoot);
    std::cout << "dim = " << dim << " for " << nRoot <<std::endl;
    if(dim==X) std::cout << "Splitting dimension of X for node " << nRoot
		      <<std::endl;
    if(dim==Y) std::cout << "Splitting dimension of Y for node " << nRoot
		      <<std::endl;
    if(dim==Z) std::cout << "Splitting dimension of Z for node " << nRoot
		      <<std::endl;
    //find median; swap with root node
    //set flag to identify splitting dimension
    //need to partition set into nodes on either side of splitting
    //    plane
    ///... stuff
    //now load each set into the kdTree
  std::cout << "kdTree[2,4]=" << kdTree[2] << ',' << kdTree[4] <<std::endl;
    medianRootSwapAndPartition(nRoot,dim);
  std::cout << "kdTree[2,4]=" << kdTree[2] << ',' << kdTree[4] <<std::endl;
    //balance left and right children
    std::cout << "kdBalance(" << nRoot << ") calling kdBalance(" << 2*nRoot
	 << ")\n";
    kdBalance(2*nRoot);
    if(nRoot*2+1<kdSize)
      kdBalance(2*nRoot+1);
  }
  else {
    kdTree[nRoot]->flag=CHILD;
  }
}

//creates ordered list of photons in dim to find median
//swaps median in dim with root if the two are different.
//swaps left and right nodes in tree of nRoot according to
//  location relative to median; orders tree
void PhotonMap::medianRootSwapAndPartition(int nRoot,KD_DIM dim) {
  std::cout << "medianRootSwapAndPartition(" << nRoot << "," << dim << ")\n";
  list<Photon **> sortedPList;
  list<Photon **> leftList;
  list<Photon **> rightList;
  //generate sorted list in dimension dim of all photons below nRoot
  generateList(&sortedPList, nRoot, dim);
  for(list<Photon **>::iterator itList = sortedPList.begin();
      itList!=sortedPList.end();
      itList++) {
    std::cout << "sortedPList elt = (" << (**itList)->x << ','
	 << (**itList)->y << ','
	 << (**itList)->z << ")\n";
  }
  list<Photon **>::iterator itList = sortedPList.begin();
  int listSize = sortedPList.size()/2;
  std::cout << "mrswp+kdTree[2,4]=" << kdTree[2] << ',' << kdTree[4] <<std::endl;
  if(listSize) {
    for(int n=0;n<listSize;(itList++)) {
      n++;
      if(n!=listSize) {
	leftList.push_back(*itList);
      }
    }
    list<Photon **>::iterator rightListStart = itList;
    rightListStart++;

  for(list<Photon **>::iterator itList = sortedPList.begin();
      itList!=sortedPList.end();
      itList++) {
    std::cout << "sortedPList elt = (" << (**itList)->x << ','
	 << (**itList)->y << ','
	 << (**itList)->z << ")\n";
  }

  std::cout << "mrsqp++kdTree[2,4]=" << kdTree[2] << ',' << kdTree[4] <<std::endl;

    //itList is now pointing at the median of the list; swap with root
    if((**itList)!=kdTree[nRoot]) {
      //swap them if unequal
      Photon * temp=kdTree[nRoot];
      std::cout << "kdTree[" << nRoot << "]=" << kdTree[nRoot] << ',' << kdTree[2] << ',' << kdTree[4] <<std::endl;
      kdTree[nRoot] = *(*itList);
      std::cout << "kdTree[" << nRoot << "]=" << kdTree[nRoot] << ',' << kdTree[2] << ',' << kdTree[4] <<std::endl;
      *(*itList) = temp;
      std::cout << "kdTree[" << nRoot << "]=" << kdTree[nRoot] << ',' << kdTree[2] << ',' << kdTree[4] <<std::endl;
    }
  std::cout << "1-mrsqp++kdTree[2,4]=" << kdTree[2] << ',' << kdTree[4] <<std::endl;
    kdTree[nRoot]->flag=dim;
    int  n=0;
  std::cout << "1-mrsqp++kdTree[2,4]=" << kdTree[2] << ',' << kdTree[4] <<std::endl;
    for(itList=rightListStart; itList!=sortedPList.end(); itList++) {
      rightList.push_back(*itList);
    }
    //now load each set into the kdTree
  std::cout << "1mrsqp++kdTree[2,4]=" << kdTree[2] << ',' << kdTree[4] <<std::endl;
    fillTree(nRoot*2,&leftList);
  std::cout << "1mrsqp++kdTree[2,4]=" << kdTree[2] << ',' << kdTree[4] <<std::endl;
    fillTree(nRoot*2+1,&rightList);
  std::cout << "2mrsqp++kdTree[2,4]=" << kdTree[2] << ',' << kdTree[4] <<std::endl;
  }
}

//fills tree with photons in vector, in unspecified order
//removes them from the vector once they've been added.
void PhotonMap::fillTree(int nRoot, list<Photon **> * pNodes) {
  std::cout << "filltree( " << nRoot << "):" << "kdTree[2,4]=" << kdTree[2] << ',' << kdTree[4] <<std::endl;
  if(!pNodes->empty())
    {
      if(nRoot<kdSize)
	{ //at least one node to be transferred, space for it exists.
	  kdTree[nRoot]=*(pNodes->front());
	  pNodes->pop_front();
	  if(nRoot*2<kdSize) 
	    { //child nodes/trees exist
	      fillTree(nRoot*2,pNodes);
	      if(nRoot*2<kdSize+1)
	      fillTree(nRoot*2+1,pNodes);
	    }
	}
    }
#if 0
  else
    {
      //      kdTree[nRoot]=0;
      if(2*nRoot<kdSize)
	{
	  fillTree(nRoot*2,pNodes);
	  if(2*nRoot+1<kdSize)
	    fillTree(nRoot*2+1,pNodes);
	}
    }
#endif
}


//FIXME: flags here set to be place in kdTree[] of current node
void PhotonMap::generateList(list<Photon **> * sList, int nRoot, KD_DIM dim) {
  if(!kdTree[nRoot]) return;
  std::cout << "generateList(" << sList << ',' << nRoot << ',' << dim << ")\n";
  std::cout << "kdTree[nRoot]=(" << kdTree[nRoot]->x << ',' << kdTree[nRoot]->y << ")\n";
  std::cout << "kdTree[2,4]=" << kdTree[2] << ',' << kdTree[4] <<std::endl;
  if(sList->empty()) {
    kdTree[nRoot]->flag=nRoot;
    std::cout << "Inserting root node into list with x val of " << kdTree[nRoot]->x <<std::endl;
    sList->push_back(&kdTree[nRoot]);
  }
  else {
    list<Photon **>::iterator itList;
    //    list<Photon *>::iterator itLast;
    double comp;
    switch(dim) {
    case X:
      comp=kdTree[nRoot]->x;
      //for loop finds first photon in list s.t. the photon is to the
      //  left of the rootNode in the x dimension
      for(itList=sList->begin();
	  (itList!=sList->end())
	    &&((**itList)->x < comp);
	  itList++);
      if(itList==sList->end()) { 
	sList->push_back(&kdTree[nRoot]);
      }
	 else {
      //now itLast points to right place in list
	   //           kdTree[nRoot]->flag=nRoot;
           sList->insert((itList),&kdTree[nRoot]);
	 }
      break;
    case Y:
      comp=kdTree[nRoot]->y;
      //for loop finds first photon in list s.t. the photon is to the
      //  left of the rootNode in the x dimension
      for(itList=sList->begin();
	  (itList!=sList->end())
	    &&((**itList)->y < comp);
	  itList++);
      if(itList==sList->end()) sList->push_back(&kdTree[nRoot]);
	 else {
      //now itLast points to right place in list
	   //           kdTree[nRoot]->flag=nRoot;
           sList->insert((itList),&kdTree[nRoot]);
	 }
      break;
    case Z:
      comp=kdTree[nRoot]->z;
      //for loop finds first photon in list s.t. the photon is to the
      //  left of the rootNode in the x dimension
      for(itList=sList->begin();
	  (itList!=sList->end())
	    &&((**itList)->z < comp);
	  itList++);
      if(itList==sList->end()) sList->push_back(&kdTree[nRoot]);
	 else {
      //now itLast points to right place in list
	   //           kdTree[nRoot]->flag=nRoot;
           sList->insert((itList),&kdTree[nRoot]);
	 }
      break;
    default:
      std::cerr << "PhotonMap given wrong dimension variable!\n";
      exit(1);
      break;
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
KD_DIM PhotonMap::findMaxDim(int nRoot) {
  double xMin, xMax, yMin, yMax, zMin, zMax;
  //set dimensions to be outside our allowed viewing area
  xMax=yMax=zMax=SCOPE_MIN;
  xMin=yMin=zMin=SCOPE_MAX;
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
  std::cout << xDiff << ',' << yDiff << ',' << zDiff <<std::endl;
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

//outputs a representation of the tree to stdout
void PhotonMap::outputTree(int nRoot) {
  if(kdTree[nRoot]) {
    std::cout << '(' << (*kdTree[nRoot]).flag << ":" << (*kdTree[nRoot]).x << ',' << kdTree[nRoot]->y <<
      ',' << kdTree[nRoot]->z << ')';
    if(nRoot * 2 < kdSize) {
      std::cout << '[';
      outputTree(nRoot*2);
    } 
    if(nRoot*2+1 < kdSize) {
      std::cout << ',';
      outputTree(nRoot*2+1);
    }
    std::cout << ']';
  }
}

std::ostream& PhotonMap::out(std::ostream& o){
  FILE_TYPE FileType = UNCOMPRESSED_V2;
  switch(FileType)
    {
    case UNCOMPRESSED_V2:
      o << "BEGIN_PHOTONMAP ";
      o << FileType;
      o << ' ';
      o << " END_PHOTONMAP";
      break;
    default:
      std::cerr << "unknown file type\n";
      break;
    }
  return o;
}

std::istream& PhotonMap::in(std::istream& is){
  string beginString; 
  int readFileType;
  char c;
  is >> beginString;
  if(beginString!="BEGIN_PHOTONMAP")
    {
      std::cerr << "Error reading photon map file.  No \"BEGIN_PHOTONMAP\""
	   << " string found\n";
    }
  else
    {
      is >> readFileType;
      switch(readFileType)
	{
	case UNCOMPRESSED:
	  std::cerr << "File is of antiquated type\n";
	  break;
	case UNCOMPRESSED_V2:
	  std::cerr << "Function not yet implemented\n";
	  break;
	}
    }
}
