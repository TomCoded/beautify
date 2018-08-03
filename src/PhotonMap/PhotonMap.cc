#include "PhotonMap.h"

extern bool g_parallel;

/* constructors and destructors */
PhotonMap::PhotonMap():
  numNeighbors(200),
  maxDistDefault(2.0),
  minSearchSqr(1.0)
{}

/* do not use */
PhotonMap::PhotonMap(PhotonMap& other)
{
  numNeighbors = other.numNeighbors;
  kdSize = other.kdSize;
  minSearchSqr = other.minSearchSqr;
  cerr << "Attempt to copy Photon Map; Unimpemented\n";
  exit(1);
}

PhotonMap::~PhotonMap(){}
/* end constructors and descructors */
/* build/query interface */

/* False if not enough photons for neighbor estimate,
   Or if not in kd-tree format 
*/
bool PhotonMap::isSearchable() const {
  if( (getSize() < numNeighbors)
      ||
      (storage != KD_TREE)
      )
    return false;
  return true;
}

void PhotonMap::addPhoton(Photon &p){
  //if the photon has negligible power, return
  if(p.r < g_photonPowerLow.x
     && p.g < g_photonPowerLow.y
     && p.b < g_photonPowerLow.z
     )
    return;
  p.offset = getSize();
  p.flag = NOT_SET;
  unsortedPhotons.push_back(p);
}


int PhotonMap::getSize() const {
  return unsortedPhotons.size();
}

void PhotonMap::setNumNeighbors(const int numNeighbors) {
  this->numNeighbors = numNeighbors;
}

void PhotonMap::setMinSearch(const double minSearchSqr) {
  this->minSearchSqr = minSearchSqr;
}

void PhotonMap::setMaxDist(const double maxDist) {
  maxDistDefault=maxDist;
}

void PhotonMap::buildTree() {
  kdSize = unsortedPhotons.size()+1;
  kdTree = (Photon **) malloc(kdSize * 2 * sizeof(Photon *));

  //save ourself from embarassment later
  kdTree[0] = 0;

  for(int n=1; n<kdSize; n++) {
    kdTree[n]=&unsortedPhotons[n-1];
  }
  for(int n=kdSize; n<kdSize * 2; n++)
    kdTree[n]=0;
  kdSize=kdSize*2;
  kdBalance(1);
  storage=KD_TREE;
}

Point3Dd PhotonMap::getFluxAt(Point3Dd &loc, Point3Dd& normal){
  Photon ** close = new Photon *[numNeighbors];
  Point3Dd rval;

  for(int n=0; n<numNeighbors; n++) {
    close[n]=0;
  }

  // if photonmap really tiny, go to hell.
  if(getSize() < numNeighbors) {
    cerr << "PhotonMap not large enough; require at least " <<
      numNeighbors << " photons.\n";
    return Point3Dd(0,0,0);
  }

  double dist;
  //set maximum search distance
  maxDist = maxDistDefault;
  maxDistSqr = maxDist * maxDist;

  if(storage!=KD_TREE) {
    cerr << "Photon Map must be sorted into kdtree before calling getFlux.\n";
    exit(1);
  }

  //generate priority queue of nearest neighbors
  PhotonPriorityQueue Q(loc);

  findNearestNeighbors(1,loc,&Q);

  if(Q.getSize()) {
    //if there are any photons in the queue (near the point we're
    //    computing luminance)
    Point3Dd powSum(0,0,0);
    Point3Dd lDir;
    double myMult;

    while(Q.getSize()>numNeighbors)
      Q.pop();

    maxDistSqr = distance(Q.top(),loc);

#if DEBUG_BUILD
    static int totalQueued = 0;
    static int passes = 0;
    passes++;
    totalQueued+=Q.getSize();
    static double totalMaxDist = 0;
    totalMaxDist+= distance(Q.top(),loc);
    if(!(passes%1000)) 
    std::cout << "Q.getSize() = " << Q.getSize() << "; avg Q size so far "
	 << (double)totalQueued / (double)passes 
	 << "; maxDist = " << distance(Q.top(),loc) 
	 << "; avg maxDist = " << totalMaxDist / (double)passes 
	 << "; avg true maxDist = " 
	 << std::sqrt(totalMaxDist/(double)passes) 
	 <<std::endl;
#endif
#if 0
    if(Q.getSize() > (numNeighbors*(6.0/7.0))) 
      return Point3Dd(1,0,0);
    if(Q.getSize() > (numNeighbors*(5.0/7.0)))
      return Point3Dd(0.5,0,0);
    if(Q.getSize() > (numNeighbors*(4.0/7.0))) 
      return Point3Dd(0,1,0);
    if(Q.getSize() > (numNeighbors*(3.0/7.0)))
      return Point3Dd(0,0.5,0);
    if(Q.getSize() > (numNeighbors*(2.0/7.0))) 
      return Point3Dd(0,0,1);
    if(Q.getSize() > (numNeighbors*(1.0/7.0))) 
      return Point3Dd(0,0,0.5);
    return Point3Dd(0,0,0);
#endif

    double cosMult;
    Photon * topPhoton;
    while(Q.getSize()) {
      //add to the sum of power in the region an intesity
      //proportional the cosine of the angle between the incident 
      //direction of the photon and the angle to the eye
      topPhoton = Q.top();
      Point3Dd incident(-1*(topPhoton->dx), -1*(topPhoton->dy),
			-1*(topPhoton->dz));

      cosMult = ( incident.dot(normal) / ( incident.norm() * normal.norm() ));
      
#if 0
      static double cosTotal=0.0;
      static int cosPass = 0;
      cosPass++;
      cosTotal+=cosMult;
      if(!(cosPass%1000)) {
	std::cout << "normal is " << normal <<std::endl;
	std::cout << "incident light dir is " << incident <<std::endl;
	std::cout << "avg cosMult so far " << cosTotal / (double)cosPass <<std::endl;
      }
#endif
      Point3Dd pow(topPhoton->r * cosMult,
		   topPhoton->g * cosMult,
		   topPhoton->b * cosMult
		   );

      if(pow.x < 0) pow.x = 0;
      if(pow.y < 0) pow.y = 0;
      if(pow.z < 0) pow.z = 0;

      powSum+=pow;

      Q.pop();
    }
    //flux = intensity / area, area is on a surface so approximate
    //with circle
    double con = 1/(PI*maxDistSqr);
    rval = powSum*con;
  }
  else rval = Point3Dd(0,0,0);

  delete close;

  //  if(rval.x || rval.y || rval.z)
  //    std::cout << "Flux is " << rval <<std::endl;
  return rval;
}

Point3Dd PhotonMap::getLuminanceAt(const Point3Dd &loc, 
				   const Point3Dd &dir, 
				   Participating * medium
				   ) {
  Photon ** close = new Photon *[numNeighbors];
  Point3Dd rval;

#if 0
  static int counter=0;
#endif
#if 0
  if(!((++counter)%10000)) {
    std::cout << "Getting luminance...";
    std::cout << "based on "
	 << numNeighbors
	 << " out to a distance of " << maxDist
	 << " from map of size " << getSize() <<std::endl;
  }
#endif

  for(int n=0; n<numNeighbors; n++) {
    close[n]=0;
  }

  // if photonmap really tiny, return no local luminance
  if(getSize() < numNeighbors) return Point3Dd(0,0,0);

  double dist;
  //set maximum search distance
  maxDist = maxDistDefault;
  maxDistSqr = maxDist * maxDist;

  if(storage!=KD_TREE) {
    cerr << "Photon Map must be sorted into kdtree before calling getluminance.\n";
    exit(1);
  }

  //generate priority queue of nearest neighbors
  PhotonPriorityQueue Q(loc);

  findNearestNeighbors(1,loc,&Q);

#if 0 //warning: Can cause segfaulting!
  static int queuetotal=0;
  queuetotal+=Q.getSize();
  if(!(counter%20000)) {
    std::cout << "Queue Size about " << loc << " is " << Q.getSize() <<
      " for a mean of " << (queuetotal / counter) <<
     std::endl;
  }
#endif

  if(Q.getSize()) {
    //if there are any photons in the queue (near the point we're
    //    computing luminance)
    Point3Dd powSum(0,0,0);
    Point3Dd lDir;
    double myMult;

    while(Q.getSize()>numNeighbors)
      Q.pop();

    maxDistSqr = distance(Q.top(),loc);

    //#ifdef flat_map
#if 0
    static int totalQueued = 0;
    static int passes = 0;
    passes++;
    totalQueued+=Q.getSize();
    static double totalMaxDist = 0;
    totalMaxDist+= distance(Q.top(),loc);
    std::cout << "Q.getSize() = " << Q.getSize() << "; avg Q size so far "
	 << (double)totalQueued / (double)passes 
	 << "; maxDist = " << distance(Q.top(),loc) 
	 << "; avg maxDist = " << totalMaxDist / (double)passes <<std::endl;;
    if(Q.getSize() > 30) 
      return Point3Dd(1,0,0);
    if(Q.getSize() > 25)
      return Point3Dd(0.5,0,0);
    if(Q.getSize() > 20) 
      return Point3Dd(0,1,0);
    if(Q.getSize() > 15)
      return Point3Dd(0,0.5,0);
    if(Q.getSize() > 10) 
      return Point3Dd(0,0,1);
    if(Q.getSize() > 5) 
      return Point3Dd(0,0,0.5);
    return Point3Dd(0,0,0);
#endif

    Photon * topPhoton;
    Point3Dd pow;
    Point3Dd phaseFunc;
    while(Q.getSize()) {

      topPhoton = Q.top();

      //power of photon
      pow.x = topPhoton->r;
      pow.y = topPhoton->g;
      pow.z = topPhoton->b;

      //incident direction of photon; needed for phase function
      Point3Dd pdir(topPhoton->dx, topPhoton->dy, topPhoton->dz);

      //adjust for phase function of medium
      medium->phaseFunction(pdir,dir);

      //not an anti-photon!
      if(pow.x<0) pow.x=0;
      if(pow.y<0) pow.y=0;
      if(pow.z<0) pow.z=0;

      //Add contributions to total sum
      powSum.x += pow.x;
      powSum.y += pow.y;
      powSum.z += pow.z;

      Q.pop();
    }
    //flux = intensity / area, area is on a surface so approximate
    //with circle
    double con = 1/((4.0/3.0)*PI*maxDistSqr*sqrt(maxDistSqr));
    rval = powSum*con;
  }
  //tag this to highlight image areas where we don't have enough photons
  //  else rval = Point3Dd(0,0,3);

  delete close;

  return rval;
}

/* private helper functions */

//given the photon map, a postion, and a max search distance, returns
//(by in-out parameters)
//a heap h with the nearest photons
//phLoc is the location of the photon at the current place in the kdTree
//loc is the location we're finding nearest neighbors of
void PhotonMap::findNearestNeighbors(int phLoc, const Point3Dd &loc,PhotonPriorityQueue * Q) {
  double dist;
  if(!kdTree[phLoc]) return;
  if(2*phLoc < kdSize) {
    //if child nodes might exist
    if(kdTree[2*phLoc]) {
      //if the left child is non-empty
      //compute distance to plane from current photon
      switch(kdTree[phLoc]->flag) {
      case X:
	dist = loc.x - kdTree[phLoc]->x;
	break;
      case Y:
	dist = loc.y - kdTree[phLoc]->y;
	break;
      case Z:
	dist = loc.z - kdTree[phLoc]->z;
	break;
      default:
	cerr << "Photon "
	     << *kdTree[phLoc]
	     << " at position "
	     << phLoc
	     << " is not part of kd tree\n";
	cerr << "Pretending is of dimension x...\n";
	dist = loc.x - kdTree[phLoc]->x;
	break;
      }
      if(dist < 0) {
	//loc is to the left of the splitting plane
	//search left subtree
	findNearestNeighbors(2*phLoc,loc,Q);

	//if the distance to the splitting plane from the location
	//is less than the maximum distance we'll consider photons to
	//be neighbors at, go ahead and search the right subtree too.
	if( dist*dist < maxDistSqr ) {
	  if((phLoc*2+1 < kdSize)&&(kdTree[phLoc*2+1])) {
	    findNearestNeighbors(2*phLoc+1,loc,Q);
	  }
	}
      } else {
	//we are to the right of the splitting plane
	//search right subtree
	if((phLoc*2+1 < kdSize)&&(kdTree[phLoc*2+1])) {
	  findNearestNeighbors(2*phLoc+1,loc,Q);
	}
	//if we're close enough to the splitting plane for it to
	//matter, search left subtree too.
	if (dist * dist < maxDistSqr) {
	  if((phLoc*2 < kdSize)&&(kdTree[phLoc*2])) {
	    findNearestNeighbors(2*phLoc,loc,Q);
	  }
	}
      }
    }
  }
  //Compare the present photon to the location we're looking for.  If
  //it's within maxdist, go ahead and add it to the priority queue.
  //compute true (squared) distance to photon
  dist = distance(kdTree[phLoc],loc);
  if(dist < maxDistSqr) {
    //if the photon in the root of the kd subtree we've just worked on
    //is within maxDist of the location we're calculating
    //flux/luminance for, go ahead and throw that photon into the
    //priority queue.
    Q->add(kdTree[phLoc]);

    //now we refine maxDist for this photon, to prune the search.
    //set it to be equal to the distance to the farthest current neighbor
    //    maxDistSqr = dist;
    maxDistSqr = distance(Q->top(),loc) < minSearchSqr ? minSearchSqr : distance(Q->top(),loc);
    //    maxDistSqr = distance(Q->top(),loc);

    //don't need to actually update maxDist - do we use it for
    //anything anyway?
  }
}

#ifdef PARALLEL

//send photons from child processes to master
//maxPhotons used for max buffer size
void PhotonMap::gatherPhotons(int maxPhotons) {
  int rank, nodes;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&nodes);

  //FIXME: Dynamic buffer resizing?
  
  MPI_Barrier(MPI_COMM_WORLD);
  if(rank) {
    //slave processes
    int sendsize = getSize();
    Photon *tmp = (Photon *)malloc(maxPhotons * sizeof(Photon));
    getArrMembers(tmp);
    MPI_Send(tmp,sendsize,MPI_PHOTON,0,sendsize,MPI_COMM_WORLD);
  } else {
    //master process
    int cnt = 0, totalsize = 0;
    MPI_Status stat;
    Photon *tmp = (Photon *)malloc(maxPhotons * (nodes - 1) * sizeof(Photon));
#define TAG_HANDSHAKE 6000
    for(int i=1; i < nodes; i++) {
      do {
	MPI_Probe(i,MPI_ANY_TAG,MPI_COMM_WORLD,&stat);
	MPI_Get_count(&stat,MPI_PHOTON,&cnt);
      } while (stat.MPI_TAG == TAG_HANDSHAKE);
      MPI_Recv((tmp + totalsize),cnt,MPI_PHOTON,i,MPI_ANY_TAG,MPI_COMM_WORLD,&stat);
      totalsize += cnt;
    }
    for(int i=0; i < totalsize; i++) 
      addPhoton(*(tmp+i));
    delete tmp;
  }

}

//distribute tree to all processes
void PhotonMap::distributeTree() {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  //synchronize processes
  //  MPI_Barrier(MPI_COMM_WORLD);

  //allocate space for kdTree buffer 
  Photon * _kdTree;

  if(!rank) {
    _kdTree = new Photon[kdSize];
  }

  if(!rank) {
    for(int i=1; i<kdSize; i++) {
      if(kdTree[i]) {
	//non-null
	_kdTree[i]=*(kdTree[i]);
      } else {
	_kdTree[i].flag=NULL_PHOTON;
      }
    }
  }

  //let everything know kdSize, numNeighbors, and minSearchSqr.
  MPI_Bcast(&kdSize,1,MPI_INT, 0, MPI_COMM_WORLD);

  MPI_Bcast(&numNeighbors,1,MPI_INT,
	    0, MPI_COMM_WORLD);

  MPI_Bcast(&minSearchSqr,1,MPI_DOUBLE,
	    0, MPI_COMM_WORLD);

  if(rank)
    _kdTree = new Photon[kdSize];

  //copy kdTree into everything
  MPI_Bcast(_kdTree,kdSize,MPI_PHOTON,
	    0, MPI_COMM_WORLD
	    );

  //if not the root, dump it back into the PhotonMap's kdTree.
  if(rank) {
    for(int i=1; i<kdSize; i++) {
      //dump photons into unsortedPhotons
      unsortedPhotons.push_back(_kdTree[i]);
    }

    //now allocate space for kdTree
    kdTree = new Photon *[kdSize];
    //and set its pointers corretly
    for(int i=1; i<kdSize; i++) {
      if(unsortedPhotons[i-1].flag!=NULL_PHOTON) {
	kdTree[i] = &unsortedPhotons[i-1];
      } else
	kdTree[i] = 0;
    }
    storage=KD_TREE;
    //and we're done.
  }

  delete [] _kdTree;

}
#endif

//recursively balances the kdTree
int PhotonMap::kdBalance(int nRoot) {
  list<Photon *> leftList;
  list<Photon *> rightList;
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
    medianRootSwapAndPartition(nRoot,dim,&leftList,&rightList);
    //now load each set into the kdTree
    //    if (g_parallel) {
      // if more processes available, send work
    //    } else {
      fillTree(nRoot*2,&leftList);
      fillTree(nRoot*2+1,&rightList);
      //balance left and right children
      kdBalance(2*nRoot);
      if(nRoot*2+1<kdSize)
        kdBalance(2*nRoot+1);
      //    }
  }
  else {
    kdTree[nRoot]->flag=CHILD;
  }
}

//creates ordered list of photons in dim to find median
//swaps median in dim with root if the two are different.
//swaps left and right nodes in tree of nRoot according to
//  location relative to median; orders tree
void PhotonMap::medianRootSwapAndPartition(int nRoot,KD_DIM dim,
		list<Photon *> *leftList, list<Photon *> *rightList) {
  //  std::cout << "medianRootSwapAndPartition(" << nRoot << ',' << dim << ")\n";
  list<Photon **> sortedPList;
  //generate sorted list in dimension dim of all photons below nRoot
  generateList(&sortedPList, nRoot, dim);
  list<Photon **>::iterator itList = sortedPList.begin();
  int listSize = sortedPList.size()/2;
  if(listSize) {
    for(int n=0;n<listSize;n++) {
      if(n!=listSize) {
	(*leftList).push_back(**itList);
        itList++;
      }
    }
    //itList is now pointing at the median of the list; swap with root
    list<Photon **>::iterator itMedian = itList;
    itList++;
    for(; itList!=sortedPList.end(); itList++) {
      (*rightList).push_back(**itList);
   }
    //now do median swap
    itList=itMedian;
    if((**itList)!=kdTree[nRoot]) {
      //swap them if unequal
      Photon * temp = kdTree[nRoot];
      kdTree[nRoot] = **itList;
      **itList = temp;
    }
    kdTree[nRoot]->flag=dim;
  }
}

//fills tree with photons in vector, in unspecified order
//removes them from the vector once they've been added.
void PhotonMap::fillTree(int nRoot, list<Photon *> * pNodes) {
  if(!pNodes->empty()) {
      //at least one node to be transferred, space for it exists.
      kdTree[nRoot]=pNodes->front();
      pNodes->pop_front();
      if(nRoot*2<kdSize) {
        //child nodes/trees exist
        fillTree(nRoot*2,pNodes);
        if(nRoot*2+1<kdSize)
	  fillTree(nRoot*2+1,pNodes);
    }
  } else if(nRoot<kdSize) {
    //fill remaining children with nulls; no photons on this side of
    //median that are not already in the kd-tree.
    kdTree[nRoot]=0;
    if(nRoot*2<kdSize) {
      fillTree(nRoot*2,pNodes);
      if(nRoot*2+1<kdSize)
	fillTree(nRoot*2+1,pNodes);
    }
  }
}


//FIXME: flags here set to be place in kdTree[] of current node
void PhotonMap::generateList(list<Photon **> * sList, int nRoot,KD_DIM dim) {
  //  std::cout << "generateList(" << sList << ',' << nRoot << ',' << dim << ")\n";
  if(!kdTree[nRoot]) return;
  if(sList->empty()) {
    //    kdTree[nRoot]->flag=nRoot;
    sList->push_back(&kdTree[nRoot]);
  }
  else {
    list<Photon **>::iterator itList;
    double comp;
    switch(dim) {
    case X:
      comp=kdTree[nRoot]->x;
      //for loop finds first photon in list s.t. the photon is to the
      //  left of the rootNode in the x dimension
      for(itList=sList->begin();
	  (itList!=sList->end())
	    && ((**itList)->x < comp);
	  itList++);
      if(itList==sList->end()) { 
        sList->push_back(&kdTree[nRoot]);
      } else {
        sList->insert(itList,&kdTree[nRoot]);
      }
      break;
    case Y:
       comp=kdTree[nRoot]->y;
      //for loop finds first photon in list s.t. the photon is to the
      //  left of the rootNode in the x dimension
      for(itList=sList->begin();
	  (itList!=sList->end())
	    && ((**itList)->y < comp);
	  itList++);
      if(itList==sList->end()) { 
        sList->push_back(&kdTree[nRoot]);
      } else {
        sList->insert(itList,&kdTree[nRoot]);
      }
    break;
    case Z:
       comp=kdTree[nRoot]->z;
      //for loop finds first photon in list s.t. the photon is to the
      //  left of the rootNode in the x dimension
      for(itList=sList->begin();
	  (itList!=sList->end())
	    && ((**itList)->z < comp);
	  itList++);
      if(itList==sList->end()) { 
        sList->push_back(&kdTree[nRoot]);
      } else {
        sList->insert(itList,&kdTree[nRoot]);
      }
     break;
    default:
      cerr << "PhotonMap given wrong dimension variable!\n";
      exit(1);
      break;
    }
  }
  if(nRoot*2 < kdSize) {//has children, balance them
    generateList(sList,nRoot*2,dim);
    if(nRoot*2+1 < kdSize) 
      generateList(sList,nRoot*2+1,dim);
  }
  //  kdTree[nRoot]->flag=nRoot;
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
  // std::cout << xDiff << ',' << yDiff << ',' << zDiff <<std::endl;
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

//dump unsorted array of photons into passed buffer
void PhotonMap::getArrMembers(Photon *mule) {
  for(int i = 0; !(unsortedPhotons.empty()); unsortedPhotons.pop_back()) {
    *(mule + i) = unsortedPhotons.back();
    i++;
  }
  return;
}

//computes true squared distance from a photon to a given location
#ifndef SQR
#define SQR(x) x * x
#endif
inline double PhotonMap::distance(const Photon * p, const Point3Dd& l) {
  return SQR((p->x - l.x)) + SQR((p->y - l.y)) + SQR((p->z - l.z));
}

/* input/output interface */

//outputs a representation of the tree to stdout
void PhotonMap::outputTree(int nRoot) {
  if(kdTree[nRoot]) {
    std::cout << '(' << (*kdTree[nRoot]).flag << ":" << (*kdTree[nRoot]).x << ',' << kdTree[nRoot]->y <<
      ',' << kdTree[nRoot]->z << ')';
    if((nRoot * 2 < kdSize)&&(kdTree[nRoot*2])) {
      std::cout << '[';
      if((nRoot * 2 < kdSize)&&(kdTree[nRoot*2])) {
	outputTree(nRoot*2);
      } 
      if((nRoot*2+1 < kdSize)&&(kdTree[nRoot*2+1])) {
	std::cout << ',';
	outputTree(nRoot*2+1);
      }
      std::cout << ']';
    }
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
      o << storage
	<< ' '
	<< kdSize
	<< ' '
	<< minSearchSqr
	<< ' '
	<< numNeighbors
	<< ' '
	<< maxDist
	<< ' '
	<< maxDistSqr
	<< ' '
	<< maxDistDefault
	<< " \n";

      for(int i=1; i<kdSize; i++) {
	if(kdTree[i]) 
	  o << *kdTree[i] << '\n';
	else
	  o << '\n';
      }
      o <<std::endl;
      o << "END_PHOTONMAP";
      break;

    default:
      cerr << "unknown file type\n";
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
