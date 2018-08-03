#include <string>
#include <queue>
#include "PhotonMap.h"
#include "Defs.h"
#include <math.h>
#include <Scene/Scene.h>

extern Scene * g_Scene;

#ifndef PI
#define PI 3.14159
#endif

PhotonMap::PhotonMap():
  nSize(0),
  lastPhoton(0),
  storage(UNSORTED),
  MAXSEARCHRANGE(4),
  kdSize(0)
{}


//not complete; do not use
PhotonMap::PhotonMap(PhotonMap& other)
{
  std::cerr << "Unusual call to PhotonMap::PhotonMap(&other);\n";
  if(this!=&other)
    {
      nSize=other.nSize;
      lastPhoton=other.lastPhoton;
      storage=other.storage;
      MAXSEARCHRANGE = other.MAXSEARCHRANGE;
      kdSize=other.kdSize;
    }
}

PhotonMap::~PhotonMap()
{
  if(nSize)
    delete photonArray;
  if(kdSize)
    delete kdTree;
}

int PhotonMap::getSize()
{
  return lastPhoton-1;
}

void PhotonMap::initialize(int nSize)
{
  if(this->nSize)
    delete photonArray;
  this->nSize=nSize+1;
  if(nSize)
    {
      photonArray = new Photon[nSize];
    }
  lastPhoton=1;
}

void PhotonMap::addPhoton(Photon &p)
{
  photonArray[lastPhoton++]=p;
#ifdef DEBUG_BUILD
  if(!(lastPhoton % (500)))
    {
      std::cout << "Photon Map at size " 
	   << lastPhoton
	   <<std::endl;
    }
#endif
}

//converts stored photons to a kd-tree format
void PhotonMap::buildTree()
{ 
  kdSize = lastPhoton;
  kdTree = new Photon*[lastPhoton];
  for(int n=1; n<kdSize; n++)
    {
      kdTree[n]=&photonArray[n];
    }
  kdBalance(1);
  storage=KD_TREE;
}

void PhotonMap::outputTree(int rootNode)
{
  std::cout << rootNode << ' ';// << photonArray[rootNode];
  if(rootNode*2+1<kdSize)
    {
      std::cout << " {";// << rootNode*2 << ' ';
      outputTree(rootNode*2);
      std::cout << ',';// << rootNode*2+1 << ' ';
      outputTree(rootNode*2+1);
      std::cout << "} ";
    }
}

//recursively balances the kdTree
int PhotonMap::kdBalance(int nRoot)//nMin, int nMax)
{
  if(!kdTree[nRoot]) return -1;
  if(nRoot*2+1<kdSize)
    { //has children
      //find splitting dimension
      int dim=findMaxDim(nRoot);
      //find the median; swap with root node
      //set flag to identify splitting dimension
      list<Photon *> sortedPList;
      generateList(&sortedPList,nRoot,dim);
      list<Photon *>::iterator itList=sortedPList.begin();
      int nMedianLoc;
      for(nMedianLoc=0; nMedianLoc<(sortedPList.size()/2); nMedianLoc++)
	{
	  itList++;
	}
      Photon * medianPhoton = (*itList);
      int median=medianPhoton->flag;
      int nHeight=getHeightOf(nRoot);
      if(nRoot!=median)
	{
	  Photon * pTemp = kdTree[median];
	  kdTree[median] = kdTree[nRoot];
	  kdTree[nRoot] = pTemp;
	}
      kdTree[nRoot]->flag=dim;
      //need to partition set into nodes on either side
      // of splitting plane.
      vector<Photon *> leftNodes;
      vector<Photon *> rightNodes;
      itList=sortedPList.begin();
      for(int n=0;
	  itList!=sortedPList.end();
	  n++)
	{
	  if((*itList)!=medianPhoton)
	    {
	      if(n<nMedianLoc)
		leftNodes.push_back(*itList);
	      else
		rightNodes.push_back(*itList);
	    }
	  itList++;
	}
#if 0
      splitKDNodes(nRoot,dim,nRoot*2,&leftNodes,&rightNodes);
      splitKDNodes(nRoot,dim,nRoot*2+1,&leftNodes,&rightNodes);
      list<Photon *> testList;
      generateList(&testList,nRoot,dim);
      kdTree[nRoot]->flag=dim;
      vector<Photon *>::iterator itNodes;
#endif
      //now load each set into the kdTree
      fillTree(nRoot*2,&leftNodes);
      fillTree(nRoot*2+1,&rightNodes);
      //the node.
      kdBalance(2*nRoot); //left child
      kdBalance(2*nRoot+1); //right child
    }
  else
    {
      kdTree[nRoot]->flag=DIM_CHILD;
    }
}

int PhotonMap::getHeightOf(int nRoot)
{
  if(2*nRoot<kdSize)
    {
      if(2*nRoot+1<kdSize)
	{
	  if(getHeightOf(2*nRoot)>getHeightOf(2*nRoot+1))
	    return 1+getHeightOf(2*nRoot);
	  else return 1+getHeightOf(2*nRoot+1);
	}
      else return getHeightOf(2*nRoot);
    }
  else return 0;
}

//fills tree with photons in vector, in unspecified order
//removes them from the vector once they've been added.
void PhotonMap::fillTree(int nRoot, vector<Photon *> * pNodes)
{
  if(!pNodes->empty())
    {
      if(!nRoot<kdSize)
	{ //at least one node to be transferred, space for it exists.
	  kdTree[nRoot]=(*pNodes)[pNodes->size()-1];
	  pNodes->pop_back();
	  if(nRoot*2+1<kdSize) 
	    { //child nodes/trees exist
	      fillTree(nRoot*2,pNodes);
	      fillTree(nRoot*2+1,pNodes);
	    }
	}
    }
  else
    {
      kdTree[nRoot]=0;
      if(2*nRoot+1<kdSize)
	{
	  fillTree(nRoot*2,pNodes);
	  fillTree(nRoot*2+1,pNodes);
	}
    }
}

//finds dimension with greatest different between endpoints
int PhotonMap::findMaxDim(int rootNode)
{
#define SCOPE_MIN 100000000
  //find the dimension in which the points vary most
  double xMax, yMax, zMax, xMin, yMin, zMin;
  xMax=yMax=zMax=-SCOPE_MIN;
  xMin=yMin=zMin=SCOPE_MIN;  
  fillDimSpreads(rootNode,
		 xMin,xMax,
		 yMin,yMax,
		 zMin,zMax
		 );
  int dim=0;
  if((xMax-xMin)>(yMax-yMin))
    {
      if((xMax-xMin)>(zMax-zMin))
	{
	  dim=DIM_X;
	}
      else
	{
	  dim=DIM_Z;
	}
    }
  else
    {
      if((yMax-yMin)>(zMax-zMin))
	{
	      dim=DIM_Y;
	}
      else
	dim=DIM_Z;
    }
  return dim;
}

//used by findMaxDim
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

void PhotonMap::generateList(list<Photon *> * sList, 
			     int rootNode,
			     int dim)
{
  if(!kdTree[rootNode]) return;
  if(sList->empty())
    {
      kdTree[rootNode]->flag=rootNode;
      sList->push_back(kdTree[rootNode]);
    }
  else
    {
      list<Photon *>::iterator itList;
      double comp;
      switch(dim)
	{
	case DIM_X:
	  comp=kdTree[rootNode]->x;
	  //for loop finds first photon in list s.t. 
	  // the photon is to the left of the rootNode in the x dimension.
	  for(itList=sList->begin();
	      (itList!=sList->end())
		&&((*itList)->x < comp);
	      itList++);
	  //now itList points to right place in list.
	  kdTree[rootNode]->flag=rootNode;
	  //insert the root node into the list after the first found
	  // photon to its left.
	  sList->insert(itList,kdTree[rootNode]);
	  break;
	case DIM_Y:
	  comp=kdTree[rootNode]->y;
	  for(itList=sList->begin();
	      (itList!=sList->end())
		&&((*itList)->y < comp);
	      itList++);
	  //now itList points to right place in list.
	  kdTree[rootNode]->flag=rootNode;
	  
	  sList->insert(itList,kdTree[rootNode]);
	  break;
	case DIM_Z:
	  comp=kdTree[rootNode]->z;
	  for(itList=sList->begin();
	      (itList!=sList->end())
		&&((*itList)->z < comp);
	      itList++);
	  //now itList points to right place in list.
	  kdTree[rootNode]->flag=rootNode;
	  sList->insert(itList,kdTree[rootNode]);
	  break;
	default:
	  std::cerr << "generateList() with invalid dim!\n";
	  exit(1);
	}
    }
  if(rootNode*2+1 < kdSize) //has children, balance them
    {
      generateList(sList,rootNode*2,dim);
      generateList(sList,rootNode*2+1,dim);
    }
  kdTree[rootNode]->flag=rootNode;
}

//recursively puts pointers to all photons that are children
//of rootNode into vectors of leftNodes or rightNodes, depending
//on the relative location of these photons in splitting
//dimension dim.
void PhotonMap::splitKDNodes(int splitNode, int dim, int rootNode, 
			     vector<Photon *> * leftNodes,
			     vector<Photon *> * rightNodes
			     )
{
  if(2*rootNode+1 < kdSize) //if child nodes exist
    { //sort left child
      splitKDNodes(splitNode,dim,2*rootNode,leftNodes,rightNodes);
      //sort right child
      splitKDNodes(splitNode,dim,2*rootNode+1,leftNodes,rightNodes);
    }
  //compare this node to splitNode in dimension dim
  {
    double dist;
    switch(dim)
      {
      case DIM_X:
	dist = kdTree[rootNode]->x - kdTree[splitNode]->x;
	break;
      case DIM_Y:
	dist = kdTree[rootNode]->y - kdTree[splitNode]->y;
	break;
      case DIM_Z:
	dist = kdTree[rootNode]->z - kdTree[splitNode]->z;
	break;
      default:
	std::cerr << "PhotonMap::splitKDNodes called with invalid
 splitting dimension.\n";
	exit(1);
	}
    if(dist<0) //"left" node
      leftNodes->push_back(kdTree[rootNode]);
    else
      rightNodes->push_back(kdTree[rootNode]);
  }
}

//quicksorts our photon map
void PhotonMap::quickSort()
{
  qSort(1,lastPhoton);
  storage=QUICKSORT_X;
  double xMin,yMin,zMin;
  double xMax,yMax,zMax;
  xMin=yMin=zMin=10000;
  xMax=yMax=zMax=-10000;
  double maxIntensity=0.0;
  for(int n=1; n<lastPhoton; n++)
    {
      if(photonArray[n].x > xMax)
	xMax=photonArray[n].x;
      if(photonArray[n].y > yMax)
        yMax=photonArray[n].y;
      if(photonArray[n].z > zMax)
	zMax=photonArray[n].z;
      if(photonArray[n].x < xMin)
	xMin=photonArray[n].x;
      if(photonArray[n].y < yMin)
	yMin=photonArray[n].y;
      if(photonArray[n].z < zMin)
	zMin=photonArray[n].z;
      if(photonArray[n].r > maxIntensity)
	maxIntensity = photonArray[n].r;
      if(photonArray[n].g > maxIntensity)
	maxIntensity = photonArray[n].g;
      if(photonArray[n].b > maxIntensity)
	maxIntensity = photonArray[n].b;
    }
  double volume = (xMax-xMin)*(yMax-yMin)*(zMax-zMin);
  // a little bigger than you'd think, to keep things realistic.
  if(volume<0.10) volume=0.10;
  //  MAXSEARCHRANGE = RANGEMULT * NUMNEIGHBORS * (volume/lastPhoton);
  //  //use average
  //  MAXSEARCHRANGE = maxIntensity * NUMNEIGHBORS;
						       //volume per
						       //photon
  if(MAXSEARCHRANGE>LIMMAXSEARCHRANGE)
    MAXSEARCHRANGE=LIMMAXSEARCHRANGE;
#ifdef DEBUG_BUILD
  std::cout << "MAXSEARCHRANGE in photonmap set to " << MAXSEARCHRANGE <<
 std::endl;
  std::cout << "Volume of scene is " << volume <<std::endl;
  std::cout << "Photon Map Size " << lastPhoton <<std::endl;
#endif
}

//sorts the photons between nMin and nMax in dimension X 
void PhotonMap::qSort(int nMin, int nMax)
{ //copied & modified slightlyfrom Sedgewick
  int i=nMin-1; int j=nMax; Photon v = photonArray[nMax];
  if(nMax<=(nMin+1)) return;
  for(;;)
    {
      while (photonArray[++i].x < v.x);
      while (v.x < photonArray[--j].x) if (j==1) break;
      if (i >= j) break;
      Photon temp=photonArray[i];
      photonArray[i]=photonArray[j];
      photonArray[j]=temp;
    }
  Photon temp=photonArray[i];
  photonArray[i]=photonArray[nMax];
  photonArray[nMax]=temp;
  qSort(nMin,i-1);
  qSort(i+1,nMax);
}

void PhotonMap::qSortY(int nMin, int nMax)
{ //copied & modified slightlyfrom Sedgewick
  int i=nMin-1; int j=nMax; Photon v = photonArray[nMax];
  if(nMax<=(nMin+1)) return;
  for(;;)
    {
      while (photonArray[++i].y < v.y);
      while (v.y < photonArray[--j].y) if (j==1) break;
      if (i >= j) break;
      Photon temp=photonArray[i];
      photonArray[i]=photonArray[j];
      photonArray[j]=temp;
    }
  Photon temp=photonArray[i];
  photonArray[i]=photonArray[nMax];
  photonArray[nMax]=temp;
  qSortY(nMin,i-1);
  qSortY(i+1,nMax);
}

void PhotonMap::qSortZ(int nMin, int nMax)
{ //copied & modified slightlyfrom Sedgewick
  int i=nMin-1; int j=nMax; Photon v = photonArray[nMax];
  if(nMax<=(nMin+1)) return;
  for(;;)
    {
      while (photonArray[++i].z < v.z);
      while (v.z < photonArray[--j].z) if (j==1) break;
      if (i >= j) break;
      Photon temp=photonArray[i];
      photonArray[i]=photonArray[j];
      photonArray[j]=temp;
    }
  Photon temp=photonArray[i];
  photonArray[i]=photonArray[nMax];
  photonArray[nMax]=temp;
  qSortZ(nMin,i-1);
  qSortZ(i+1,nMax);
}

bool PhotonMap::checkKDTree(int nNode)
{
  if(2*nNode+1<kdSize)
    {
      if(kdTree[nNode]->flag>DIM_Z)
	return false;
      else
	{
	  return checkKDTree(2*nNode)&&(checkKDTree(2*nNode+1));
	}
    }
  return true;
}

//legacy
//sorts the photons between nMin and nMax in dimension dim
//used many times in kdBuildTree
void PhotonMap::kdDimSort(int nMin, int nMax, int dim)
{}
#if 0
  if((nMax-nMin)>1)
    {
      //pick pivot from randomly distributed photons
      Photon * pivot = &photonArray[nMin];
      int pivotPos=0;
      //find pivot's final location
      switch(dim)
	{
	case DIM_X:
	  for(int n=nMin; n<nMax; n++)
	    {
	      if(photonArray[n].x<pivot->x)
		pivotPos++;
	    }
	  break;
	case DIM_Y:
	  for(int n=nMin; n<nMax; n++)
	    {
	      if(photonArray[n].y<pivot->y)
		pivotPos++;
	    }
	  break;
	case DIM_Z:
	  for(int n=nMin; n<nMax; n++)
	    {
	      if(photonArray[n].z<pivot->z)
		pivotPos++;
	    }
	  break;
	}
      //at this point, pivotPos has the final location.
      //put the pivot there
      Photon tmp = photonArray[pivotPos+nMin];
      photonArray[pivotPos+nMin]=photonArray[nMin];
      photonArray[nMin]=tmp;
      pivot = &photonArray[pivotPos+nMin];
      switch(dim)
	{
	case DIM_X:
	  for(int n=nMin; n<(nMin+pivotPos); n++)
	    {
	      if(photonArray[n].x>pivot->x)
		{ //find an element on the right smaller than the
		  //pivot
		  int n2;
		  for(n2=nMin+pivotPos; 
		      (n2<nMax)&&(photonArray[n2].x>pivot->x);
		      n2++);
		  //then swap them
		  Photon temp = photonArray[n2];
		  photonArray[n2] = photonArray[n];
		  photonArray[n] = temp;
		}
	    }
	  break;
	case DIM_Y:
	  for(int n=nMin; n<(nMin+pivotPos); n++)
	    {
	      if(photonArray[n].y>pivot->y)
		{ //find an element on the right smaller than the
		  //pivot
		  int n2;
		  for(n2=nMin+pivotPos; 
		      (n2<nMax)&&(photonArray[n2].y>pivot->y);
		      n2++);
		  //then swap them
		  Photon temp = photonArray[n2];
		  photonArray[n2] = photonArray[n];
		  photonArray[n] = temp;
		}
	    }
	  break;
	case DIM_Z:
	  for(int n=nMin; n<(nMin+pivotPos); n++)
	    {
	      if(photonArray[n].z>pivot->z)
		{ //find an element on the right smaller than the
		  //pivot
		  int n2;
		  for(n2=nMin+pivotPos; 
		      (n2<nMax)&&(photonArray[n2].z>pivot->z);
		      n2++);
		  //then swap them
		  Photon temp = photonArray[n2];
		  photonArray[n2] = photonArray[n];
		  photonArray[n] = temp;
		}
	    }
	  break;
	}
    }
}
#endif

//precondition: g_Scene defined.
//prints projection onto x-y plane of all photons.
//not perfect now.
void PhotonMap::showMap()
{
  double dleft, dright, dtop, dbottom;
  dleft=0; dright=0; dtop=dbottom=0;
  for(int p=1; p<lastPhoton; p++)
    {
      if(photonArray[p].x < dleft) dleft=photonArray[p].x;
      if(photonArray[p].x > dright) dright=photonArray[p].x;
      if(photonArray[p].y > dtop) dtop=photonArray[p].y;
      if(photonArray[p].y < dbottom) dbottom=photonArray[p].y;
    }
  double dwidth=dright-dleft;
  double dheight=dtop-dbottom;
  int width = g_Scene->getWindowWidth();
  int height = g_Scene->getWindowHeight();
  for(int i=0; i<height; i++)
    for(int j=0; j<width; j++)
      {
	for(int p=1; p<lastPhoton; p++)
	  {
	    if(((((photonArray[p].x-dleft)/dwidth)-((double)j/(double)width)))<0)
	      if(((((photonArray[p].y-dbottom)/dheight)-((double)i/(double)height)))<0)
		{
		  std::cout << (photonArray[p].x-dleft)/dwidth <<std::endl;
		  std::cout << (double)j/(double)width <<std::endl;
		  g_Scene->putPixel(j,i,1.0,1.0,1.0);
		  break;
		}
	  }
      }
}

//for volumetric photon map
Point3Dd PhotonMap::getLuminanceAt(Point3Dd &p)
{
  // determine n nearest neighbors
  int nNeighbors = NUMNEIGHBORS;
  Photon * close[NUMNEIGHBORS];
  for(int n=0; n<NUMNEIGHBORS; n++)
    close[n]=NULL;
  if(lastPhoton < 2) return Point3Dd(0,0,0);
  int nLast=0;
  close[0]=&photonArray[1];
  double dist;
  if(storage==UNSORTED)
    { //unsorted not debugged yet; compare to quicksort
      for(int n=1;n<nSize;n++)
	{
	  dist = distance(photonArray[n],p);
	  for(int n=1; (n<=NUMNEIGHBORS-1)&&close[n]; n++)
	    {
	      if(dist
		 <=
		 distance(*(close[n]),p)
	     )
		{ //found closer neighbor
		  for(int n2=NUMNEIGHBORS-1; n2>=1; n2--)
		    {
		      close[n2]=close[n2-1];
		    }
		  close[0]=&photonArray[n];
		}
	    }
	}
      //At this point, we have the n nearest neighbors.
      //we need to compute the flux.
      double maxDist = distance(*close[0],p);
      int nClose=NUMNEIGHBORS;
      for(int n=0; n<nClose; n++)
	{
	  if(distance(*close[n],p)>MAXSEARCHRANGE)
	    nClose--;
	}
       for(int n=1; n<nClose; n++)
	if(distance(*close[n],p)<maxDist)
	  maxDist=distance(*close[n],p);
      double con = 1/((4.0/3.0)*PI*(maxDist)*sqrt(maxDist));
      Point3Dd powSum(close[0]->r,
		      close[0]->g,
		      close[1]->b
		      );
      for(int n=1; n<NUMNEIGHBORS; n++)
	{
	  powSum.x += close[n]->r;
	  powSum.y += close[n]->g;
	  powSum.z += close[n]->b;
	}
      return powSum*con*POWERMULT;
    }
  else if(storage==KD_TREE) //the array is a kd-tree
    { 
      //generate priority queue of nearest photons
      //      priority_queue<Photon *> Q;
      PhotonPriorityQueue Q(p);
      this->maxdist=1.5;
      kdNN(1,p,&Q);
      if(Q.getSize())
	{
	  Point3Dd powSum(0,0,0);
	  Point3Dd lDir;
	  double myMult;
	  while(Q.getSize()>NUMNEIGHBORS)
	    Q.pop();
	  maxdist=sqrt(distance(*Q.top(),p));
#ifdef DEBUG_BUILD
	  static int numPhotons;
	  numPhotons=Q.getSize();
#endif
	  while(Q.getSize())
	    {
	      //intensity proportional to cosine between angle of
	      //incidence and angle from eye
	      powSum.x += (Q.top()->r);
	      powSum.y += (Q.top()->g);
	      powSum.z += (Q.top()->b);
	      Q.pop(); 
	    }
	  double con = 1/((4.0/3.0)*PI*(this->maxdist)*sqrt(this->maxdist));
#ifdef DEBUG_BUILD
	  //keep track of aggregate statistics
	  static int debug_flux_cycle=0;
	  static int debug_nPhotons=0;
	  static double debug_maxDist=0;
	  static Point3Dd meanPowSum(0,0,0);
	  debug_maxDist+=maxdist;
	  meanPowSum+=powSum*con;
	  debug_flux_cycle++;
	  debug_nPhotons+=numPhotons;
	  if(!(debug_flux_cycle%1000))
	    {
	      std::cout << "Mean of " << debug_nPhotons/1000
		   << " photons used per pixel"
		   << " and meanPowSum=" << meanPowSum
		   << " yielding mean intensity (" 
		   << meanPowSum.x/debug_flux_cycle << ','
		   << meanPowSum.y/debug_flux_cycle << ','
		   << meanPowSum.z/debug_flux_cycle << ')'
		   << " and average maxdist " 
		   << debug_maxDist/debug_flux_cycle
		   << "after " << debug_flux_cycle
		   << " pixels."
		   <<std::endl;
	      debug_nPhotons=0;
	      debug_flux_cycle=0;
	      debug_maxDist=0;
	      meanPowSum=Point3Dd(0,0,0);
	    }
#endif
	  return powSum*con;
	}
      else return Point3Dd(0,0,0);
    }
  else if(storage=QUICKSORT_X) //array is quicksorted along dimension x
    //currently only searches upwards
    { //Find photon closest to x coordinate.
      //currently only looks for photons in the positive x direction
      int nCentroid;
      for(nCentroid=1; nCentroid<lastPhoton; nCentroid++)
	  if(photonArray[nCentroid].x>p.x) break;
      int nStart;
      for(nStart=nCentroid;
	  nStart&&(photonArray[nStart].x>(p.x-MAXSEARCHRANGE)); nStart--);
      //scan through photons in that vicinity, throwing closest ones
      //into an array
      int nClose=0;
      bool found=false;
      for(int n=nStart; (n<lastPhoton)&&(photonArray[n].x<(p.x+MAXSEARCHRANGE)); n++)
	{
	  dist=distance(photonArray[n],p);
	  if(dist<MAXSEARCHRANGE)
	    {
	      found=false;
	      if(!nClose)
		{
		  close[0]=&photonArray[n];
		  nClose++;
		}
	      else for(int j=0; (!found)&&(nClose>j)&&(j<NUMNEIGHBORS); j++)
		{
		  if(dist <= distance((*close[j]),p))
		    { //closer neighbor
		      found=true;
		      if(nClose <= NUMNEIGHBORS)
			{ //if the array isn't full yet
			  //			  std::cout << "Adding at position " << j <<std::endl;;
			  nClose++;
			}
		      for(int k=j; k<nClose-1; k++)
			{
			  close[k+1]=close[k];
			}
		      close[j]=&photonArray[n];
		    }
		}
	    }
	}
      //At this point, we have the n nearest neighbors.
      //we need to compute the flux.
      if(nClose)
	{
	  double maxDist = distance(*close[0],p);
	  for(int n=1; n<nClose; n++)
	    {
	      if(distance(*(close[n]),p)>maxDist)
		maxDist=distance(*close[n],p);
	    }
	  double con = 1/((4.0/3.0)*PI*maxDist*sqrt(maxDist));
	  Point3Dd powSum(0,0,0);
	  for(int n=0; n<nClose-1; n++)
	    {
	      powSum.x += (close[n]->r);
	      powSum.y += (close[n]->g);
	      powSum.z += (close[n]->b);
	    }

#ifdef DEBUG_BUILD
      //keep track of aggreagate statistics
      static int debug_flux_cycle=0;
      static int debug_nPhotons=0;
      static double debug_maxDist=0;
      static Point3Dd meanPowSum(0,0,0);
      debug_maxDist+=maxDist;
      meanPowSum+=powSum*con;
      debug_flux_cycle++;
      debug_nPhotons+=nClose-1;
      if(!(debug_flux_cycle%1000))
	{
	  std::cout << "Mean of " << debug_nPhotons/1000
	       << " photons used per pixel"
	       << " and meanPowSum=" << meanPowSum
	       << " yielding mean intensity (" 
	       << meanPowSum.x/debug_flux_cycle << ','
	       << meanPowSum.y/debug_flux_cycle << ','
	       << meanPowSum.z/debug_flux_cycle << ')'
	       << " and average maxdist " 
	       << debug_maxDist/debug_flux_cycle
	       << "after " << debug_flux_cycle
	       << " pixels."
	       <<std::endl;
	  debug_nPhotons=0;
	  debug_flux_cycle=0;
	  debug_maxDist=0;
	  meanPowSum=Point3Dd(0,0,0);
	}
#endif

      if(powSum.x>1) powSum.x=1;
      if(powSum.y>1) powSum.y=1;
      if(powSum.z>1) powSum.z=1;
      return powSum;
      powSum = powSum*con;
      return powSum;
	}
      else 
	{
	  return Point3Dd(0,0,0);
	}
    }
}

Point3Dd PhotonMap::getFluxAt(Point3Dd &p,Point3Dd &normal)
{
  // determine n nearest neighbors
  int nNeighbors = NUMNEIGHBORS;
  Photon * close[NUMNEIGHBORS];
  for(int n=0; n<NUMNEIGHBORS; n++)
    close[n]=NULL;
  if(lastPhoton < 2) return Point3Dd(0,0,0);
  int nLast=0;
  close[0]=&photonArray[1];
  double dist;
  if(storage==UNSORTED)
    { //unsorted not debugged yet; compare to quicksort
      for(int n=1;n<nSize;n++)
	{
	  dist = distance(photonArray[n],p);
	  for(int n=1; (n<=NUMNEIGHBORS-1)&&close[n]; n++)
	    {
	      if(dist
		 <=
		 distance(*(close[n]),p)
	     )
		{ //found closer neighbor
		  for(int n2=NUMNEIGHBORS-1; n2>=1; n2--)
		    {
		      close[n2]=close[n2-1];
		    }
		  close[0]=&photonArray[n];
		}
	    }
	}
      //At this point, we have the n nearest neighbors.
      //we need to compute the flux.
      double maxDist = distance(*close[0],p);
      int nClose=NUMNEIGHBORS;
      for(int n=0; n<nClose; n++)
	{
	  if(distance(*close[n],p)>MAXSEARCHRANGE)
	    nClose--;
	}
       for(int n=1; n<nClose; n++)
	if(distance(*close[n],p)<maxDist)
	  maxDist=distance(*close[n],p);
      double con = 1/(PI*(maxDist));
      Point3Dd powSum(close[0]->r,
		      close[0]->g,
		      close[1]->b
		      );
      for(int n=1; n<NUMNEIGHBORS; n++)
	{
	  powSum.x += close[n]->r;
	  powSum.y += close[n]->g;
	  powSum.z += close[n]->b;
	}
      return powSum*con*POWERMULT;
    }
  else if(storage==KD_TREE) //the array is a kd-tree
    { 
      //generate priority queue of nearest photons
      //      priority_queue<Photon *> Q;
      PhotonPriorityQueue Q(p);
      this->maxdist=1.5;
      kdNN(1,p,&Q);
      if(Q.getSize())
	{
	  Point3Dd powSum(0,0,0);
	  Point3Dd lDir;
	  double myMult;
	  while(Q.getSize()>NUMNEIGHBORS)
	    Q.pop();
	  maxdist=sqrt(distance(*Q.top(),p));
#ifdef DEBUG_BUILD
	  static int numPhotons;
	  numPhotons=Q.getSize();
#if 0
	  std::cout << "maxDist = " 
	       << maxdist
	       <<std::endl;
#endif
#endif
	  while(Q.getSize())
	    {
#if 0
	      std::cout << "Q.top.colors = "
		   << Q.top()->r << ','
		   << Q.top()->g << ',' 
		   << Q.top()->b <<std::endl;
#endif
	      //intensity proportional to cosine between angle of
	      //incidence and angle from eye
	      lDir = Point3Dd(-Q.top()->dx,
			      -Q.top()->dy,
			      -Q.top()->dz
			      );
	      myMult = lDir.dot(normal) / (lDir.norm() * normal.norm());
	      myMult = fabs(myMult);
	      if(myMult<0) myMult=0;
	      powSum.x += (Q.top()->r*myMult);
	      powSum.y += (Q.top()->g*myMult);
	      powSum.z += (Q.top()->b*myMult);
	      Q.pop(); 
	    }
	  double con = 1/(PI*(this->maxdist));
#ifdef DEBUG_BUILD
	  //keep track of aggregate statistics
	  static int debug_flux_cycle=0;
	  static int debug_nPhotons=0;
	  static double debug_maxDist=0;
	  static Point3Dd meanPowSum(0,0,0);
	  debug_maxDist+=maxdist;
	  //      std::cout << "powSum=" << powSum <<std::endl;
	  meanPowSum+=powSum*con;
	  debug_flux_cycle++;
	  debug_nPhotons+=numPhotons;
	  //      std::cout << "Checking flux this many times: " << debug_flux_cycle <<std::endl;
	  if(!(debug_flux_cycle%1000))
	    {
	      std::cout << "Mean of " << debug_nPhotons/1000
		   << " photons used per pixel"
		   << " and meanPowSum=" << meanPowSum
		   << " yielding mean intensity (" 
		   << meanPowSum.x/debug_flux_cycle << ','
		   << meanPowSum.y/debug_flux_cycle << ','
		   << meanPowSum.z/debug_flux_cycle << ')'
		   << " and average maxdist " 
		   << debug_maxDist/debug_flux_cycle
		   << "after " << debug_flux_cycle
		   << " pixels."
		   <<std::endl;
	      debug_nPhotons=0;
	      debug_flux_cycle=0;
	      debug_maxDist=0;
	      meanPowSum=Point3Dd(0,0,0);
	    }
#endif
	  return powSum*con;
	}
      else return Point3Dd(0,0,0);
    }
  else if(storage=QUICKSORT_X) //array is quicksorted along dimension x
    //currently only searches upwards
    { //Find photon closest to x coordinate.
      //currently only looks for photons in the positive x direction
      int nCentroid;
      for(nCentroid=1; nCentroid<lastPhoton; nCentroid++)
	  if(photonArray[nCentroid].x>p.x) break;
      int nStart;
      for(nStart=nCentroid;
	  nStart&&(photonArray[nStart].x>(p.x-MAXSEARCHRANGE)); nStart--);
      //scan through photons in that vicinity, throwing closest ones
      //into an array
      int nClose=0;
      bool found=false;
      for(int n=nStart; (n<lastPhoton)&&(photonArray[n].x<(p.x+MAXSEARCHRANGE)); n++)
	{
	  dist=distance(photonArray[n],p);
	  if(dist<MAXSEARCHRANGE)
	    {
	      found=false;
	      if(!nClose)
		{
		  close[0]=&photonArray[n];
		  nClose++;
		}
	      else for(int j=0; (!found)&&(nClose>j)&&(j<NUMNEIGHBORS); j++)
		{
		  if(dist <= distance((*close[j]),p))
		    { //closer neighbor
#if 0
		      std::cout << "Found closer neighbor " << n << " at distance "
			   << dist 
			   << " and position "
			   << photonArray[n].x << ','
			   << photonArray[n].y << ','
			   << photonArray[n].z << '.'
			   <<std::endl;
#endif
		      found=true;
		      if(nClose <= NUMNEIGHBORS)
			{ //if the array isn't full yet
			  //			  std::cout << "Adding at position " << j <<std::endl;;
			  nClose++;
			}
		      for(int k=j; k<nClose-1; k++)
			{
			  close[k+1]=close[k];
			}
		      close[j]=&photonArray[n];
		    }
		}
	    }
	}
      //At this point, we have the n nearest neighbors.
      //we need to compute the flux.
      if(nClose)
	{
	  double maxDist = distance(*close[0],p);
	  for(int n=1; n<nClose; n++)
	    {
	      if(distance(*(close[n]),p)>maxDist)
		maxDist=distance(*close[n],p);
	    }
	  if(maxDist > MAXSEARCHRANGE)
	    std::cerr << "maxDist = " << maxDist << ", MSR = " <<
	      MAXSEARCHRANGE <<std::endl;
	  double con = 1/(PI*maxDist);
	  Point3Dd powSum(0,0,0);
	  Point3Dd lDir;
	  double myMult;
	  //do fake BRDF; dep. on cosine of incident light angle
	  //      v. surface normal
	  for(int n=0; n<nClose-1; n++)
	    {
	      //lDir = incident light vector
	      //Lambertian surface, diffuse light proportional to cosine of
	      //angle between vector opposite incident light and surface normal.
	      lDir = Point3Dd((-close[n]->dx),
			      (-close[n]->dy),
			      (-close[n]->dz)
			      );
	      myMult = lDir.dot(normal) / (lDir.norm() * normal.norm());
	      myMult=fabs(myMult);
#if 0
	      std::cout << "Between Light Ray " << lDir << " and Normal " << normal;
	      std::cout << " Cosine is " << myMult <<std::endl;
	      std::cout << "myMult = " << myMult <<std::endl;
#endif
	      powSum.x += (close[n]->r*myMult);
	      powSum.y += (close[n]->g*myMult);
	      powSum.z += (close[n]->b*myMult);
	    }

#ifdef DEBUG_BUILD
      //keep track of aggreagate statistics
      static int debug_flux_cycle=0;
      static int debug_nPhotons=0;
      static double debug_maxDist=0;
      static Point3Dd meanPowSum(0,0,0);
      debug_maxDist+=maxDist;
      //      std::cout << "powSum=" << powSum <<std::endl;
      meanPowSum+=powSum*con;
      debug_flux_cycle++;
      debug_nPhotons+=nClose-1;
      //      std::cout << "Checking flux this many times: " << debug_flux_cycle <<std::endl;
#if 0
      if(!(debug_flux_cycle%500))
	{
	  for(int i=0; i<nClose-1; i++)
	    std::cout << "close[" << i << "]=("
		 << close[i]->r << ','
		 << close[i]->g << ','
		 << close[i]->b << ")\n";
	}
#endif
      if(!(debug_flux_cycle%1000))
	{
	  std::cout << "Mean of " << debug_nPhotons/1000
	       << " photons used per pixel"
	       << " and meanPowSum=" << meanPowSum
	       << " yielding mean intensity (" 
	       << meanPowSum.x/debug_flux_cycle << ','
	       << meanPowSum.y/debug_flux_cycle << ','
	       << meanPowSum.z/debug_flux_cycle << ')'
	       << " and average maxdist " 
	       << debug_maxDist/debug_flux_cycle
	       << "after " << debug_flux_cycle
	       << " pixels."
	       <<std::endl;
	  debug_nPhotons=0;
	  debug_flux_cycle=0;
	  debug_maxDist=0;
	  meanPowSum=Point3Dd(0,0,0);
	}
#endif

#if 0
      std::cout << nClose-1 << " photons over an area " << (PI*maxDist)
	   << " or distance " << maxDist 
	   << "contributed to flux for " << p 
	   << " yielding " << powSum 
	   << " * " << con
	   << " = " << powSum*con
	   <<std::endl;
      powSum=powSum*con;
#if 0
      if(powSum.x>1) powSum.x=1;
      if(powSum.y>1) powSum.y=1;
      if(powSum.z>1) powSum.z=1;
#endif
      return powSum;
#endif
      powSum = powSum*con;
#if 0
      if(powSum.x>1) powSum.x=1;
      if(powSum.y>1) powSum.y=1;
      if(powSum.z>1) powSum.z=1;
#endif
      return powSum;
	}
      else 
	{
	  return Point3Dd(0,0,0);
	}
    }
}

//#if 0
//phLoc = integer location in array
// of photon at top of node being searched
// loc = position of photon for NN'ing in 3-space.
// Q=maxheap of nearest neighbors
void PhotonMap::kdNN(int phLoc, Point3Dd &loc, 
		     PhotonPriorityQueue * Q)
		     //		     priority_queue<Photon *> * Q)
{  
  if(!kdTree[phLoc]) return;
  //  std::cout << "kdNN(" << phLoc << "," << loc << ")\n";
  double dist;
  //check against CHILD_DIM instead?
  //  if(photonArray[phLoc].flag!=DIM_CHILD)
    if((2*phLoc+1 < kdSize)&&(kdTree[2*phLoc+1])) //if child nodes exist
    { //check them
      //compute distance to plane
      switch(kdTree[phLoc]->flag)
	{
	case DIM_X:
	  dist = kdTree[2*phLoc+1]->x
	    - kdTree[phLoc]->x;
	  break;
	case DIM_Y:
	  dist = kdTree[2*phLoc+1]->y
	    - kdTree[phLoc]->y;
	  break;
	case DIM_Z:
	  dist = kdTree[2*phLoc+1]->z
	    - kdTree[phLoc]->z;
	  break;
	default:
	  std::cerr << "Photon " 
	       << *kdTree[phLoc]
	       << " at pos " 
	       << phLoc
	       << " not part of kd tree\n";
	  std::cerr << "Is of dimension " << kdTree[phLoc]->flag <<std::endl;
	  std::cerr << "Pretending of dimension x...\n";
	  dist = kdTree[2*phLoc+1]->x - kdTree[phLoc]->x;
	  break;
	}
      if(dist < 0)
	{ //"left" of plane.  Search left subtree
	  kdNN(2*phLoc,loc,Q);
	  if( SQR(dist) < SQR(maxdist) ) 
	    { //if maxDist > distance to splitting plane
	      //search right subtree
	      kdNN(2*phLoc+1,loc,Q);
	    }
	}
      else //right of plane.  search right subtree first
	{
	  kdNN(2*phLoc+1,loc,Q);
	  if(SQR(dist) < SQR(maxdist) ) //search left subtree
	    kdNN(2*phLoc,loc,Q);
	}
    }

  //compute true squared distance from photon
  dist = distance(*kdTree[phLoc], loc);
#if 0
  std::cout << "From " << *kdTree[phLoc]
       << " to " << loc
       << ' ';
  std::cout << "sqr dist = " << dist <<std::endl;
  std::cout << "maxdist = " << maxdist <<std::endl;
#endif
  if( dist < SQR(maxdist) )
    { //if the photon is close enough
      //insert photon into queue
#if 0
      std::cout << "Found nearest neighbor " << *kdTree[phLoc] 
      	   << " to " << loc
      	   <<std::endl;
#endif
      //      Q->push((kdTree[phLoc]));
#if 0
      std::cout << "Adding to queue rgb: " 
	   << kdTree[phLoc]->r << ','
	   << kdTree[phLoc]->g << ','
	   << kdTree[phLoc]->b <<std::endl;
#endif
      Q->add(kdTree[phLoc]);
      //maxdist = distance(*(Q->top()),loc);
      maxdist=sqrt(dist);
    }
}

double PhotonMap::distance(Photon &p, Point3Dd &loc)
{
  double dx = p.x - loc.x;
  double dy = p.y - loc.y;
  double dz = p.z - loc.z;
  //don't need the square root for pseudo-distance
  return (SQR(dx)+SQR(dy)+SQR(dz));
}

std::ostream& PhotonMap::out(std::ostream& o)
{
  int FileType = UNCOMPRESSED;
  switch(FileType)
    {
    case UNCOMPRESSED:
      o << "BEGIN_PHOTONMAP ";
      o << FileType;
      o << ' ';
      o << nSize;
      o << ' ';
      o << lastPhoton;
      o << ' ';
      o << storage;
      o << ' ';
      for(int nTemp = 0; nTemp < lastPhoton; nTemp++)
	o << photonArray[nTemp];
      if(storage=KD_TREE)
	{
	  o << kdSize;
	  o << ' ';
	  for(int nTemp=0; nTemp < kdSize; nTemp++)
	    { //store offset from beginning of array; don't want
	      //actual memory address!
	      o << ' ';
	      o << kdTree[nTemp]-&photonArray[0];
	    }
	}
      o << " END_PHOTONMAP";
      break;
    default:
      std::cerr << "unknown file type\n";
      break;
    }
  return o;
}

std::istream& PhotonMap::in(std::istream& is)
{
  string beginString; int readFileType;
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
	  if(nSize) delete photonArray;
	  is >> nSize;
	  is >> lastPhoton;
	  {
	    photonArray = new Photon[lastPhoton];
	    nSize=lastPhoton;
	  }
	  is >> storage;
	  for(int nTemp = 0; nTemp < lastPhoton; nTemp++)
	    is >> photonArray[nTemp];
	  if(storage=KD_TREE)
	    {
	      is >> kdSize;
	      kdTree = new Photon*[kdSize];
	      long pOffset;
	      for(int n=0; n<kdSize; n++)
		{
		  is >> pOffset;
		  kdTree[n] = &photonArray[0]+pOffset;
		}
	    }
	  is >> beginString;
	  if(beginString!="END_PHOTONMAP")
	    {
	      std::cerr << "Error reading photon map file.  No \"END_PHOTONMAP\""
		   << " string found\n";
	      std::cerr << beginString << " found instead\n";
	    }
	  break;
	default:
	  std::cerr << "Unknown file type\n";
	  break;
	}
    }
  double xMin,yMin,zMin;
  double xMax,yMax,zMax;
  xMin=yMin=zMin=10000;
  xMax=yMax=zMax=-10000;
  double maxIntensity=0.0;
  for(int n=0; n<lastPhoton; n++)
    {
      if(photonArray[n].x > xMax)
	xMax=photonArray[n].x;
      if(photonArray[n].y > yMax)
        yMax=photonArray[n].y;
      if(photonArray[n].z > zMax)
	zMax=photonArray[n].z;
      if(photonArray[n].x < xMin)
	xMin=photonArray[n].x;
      if(photonArray[n].y < yMin)
	yMin=photonArray[n].y;
      if(photonArray[n].z < zMin)
	zMin=photonArray[n].z;
      if(photonArray[n].r > maxIntensity)
	maxIntensity = photonArray[n].r;
      if(photonArray[n].g > maxIntensity)
	maxIntensity = photonArray[n].g;
      if(photonArray[n].b > maxIntensity)
	maxIntensity = photonArray[n].b;
    }
  double volume = (xMax-xMin)*(yMax-yMin)*(zMax-zMin);
  // a little bigger than you'd think, to keep things realistic.
  //  MAXSEARCHRANGE = maxIntensity * NUMNEIGHBORS;
  //  MAXSEARCHRANGE = RANGEMULT *(NUMNEIGHBORS * (volume/lastPhoton)); //use average
						       //volume per photon
#if 0
  if(MAXSEARCHRANGE>LIMMAXSEARCHRANGE)
    MAXSEARCHRANGE=LIMMAXSEARCHRANGE;
#endif
#ifdef DEBUG_BUILD
  std::cout << "MAXSEARCHRANGE in photonmap set to " << MAXSEARCHRANGE <<
 std::endl;
  std::cout << "Volume of scene is " << volume <<std::endl;
  std::cout << "Photon Map Size " << lastPhoton <<std::endl;
#endif
  return is;
}
