#ifndef PHOTONPRIORITYQUEUE_H_
#define PHOTONPRIORITYQUEUE_H_

#include <Point3Dd.h>
#include <vector>
#include <Photon/Photon.h>
#include "PPQueueNode.h"

class PhotonPriorityQueue
{
 public:
  PhotonPriorityQueue();
  PhotonPriorityQueue(Point3Dd location);
  ~PhotonPriorityQueue();

  void setLocation(Point3Dd location);
  Photon * top();
  void add(Photon *);
  Photon * pop();
  int getSize();

 protected:

  double distance(PPQueueNode*);
  void swap(int,int);
  void siftdown(int);
  void percolate(int);

  Point3Dd location;
  std::vector<PPQueueNode *> PHeap;
};

#endif
