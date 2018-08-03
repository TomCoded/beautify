#include <Defs.h>
#include "PhotonPriorityQueue.h"

PhotonPriorityQueue::PhotonPriorityQueue():
  location(0,0,0)
{
  PPQueueNode *crap = new PPQueueNode;
  crap->value = NULL;
  crap->distance = -1;
  PHeap.push_back(crap);
}

PhotonPriorityQueue::PhotonPriorityQueue(Point3Dd location):
  location(location)
{
  PPQueueNode *crap = new PPQueueNode;
  crap->value = NULL;
  crap->distance = -1;
  PHeap.push_back(crap);
}

int PhotonPriorityQueue::getSize()
{ return PHeap.size() - 1; }

PhotonPriorityQueue::~PhotonPriorityQueue() {
  while(getSize()) {
    pop();
  }
  delete PHeap[0];
}

void PhotonPriorityQueue::setLocation(Point3Dd location)
{
  this->location=location;
}

inline void PhotonPriorityQueue::swap(int a, int b) {
  PPQueueNode * tmp = PHeap[a];
  PHeap[a] = PHeap[b];
  PHeap[b] = tmp;
}

void PhotonPriorityQueue::siftdown(int pos) {
  // pre: PHeap[pos] contains a valid number, PHeap[pos - 1] through PHeap[size - 1]
  //      satisfies the heap property
  // post: PHeap[pos] through PHeap[size - 1] satisfies the heap property
  if (PHeap.size() > (pos << 1) &&
      PHeap[pos]->distance < PHeap[pos << 1]->distance) { // L child
    swap(pos,(pos << 1));
    siftdown(pos << 1); // siftdown child
  }
  if (PHeap.size() > ((pos << 1) + 1) &&
      PHeap[pos]->distance < PHeap[(pos << 1) + 1]->distance) { // R child
    swap(pos,(pos << 1) + 1);
    siftdown((pos << 1) + 1);
  }
}

void PhotonPriorityQueue::percolate(int pos) {
  // pre: PHeap[1] through PHeap[pos - 1] a heap, PHeap[pos] contains
  //      some data
  // post: PHeap[1] through PHeap[pos] a heap
  if (pos >> 1 && PHeap[pos]->distance > PHeap[pos >> 1]->distance) {
    swap(pos,pos >> 1);
    percolate(pos >> 1); // percolate parent
  }
  return;
}

void PhotonPriorityQueue::add(Photon * newPhoton)
{
  PPQueueNode * newPQN = new PPQueueNode;
  newPQN->value=newPhoton;
  newPQN->distance=distance(newPQN);
  PHeap.push_back(newPQN);
  percolate(getSize());
}

Photon * PhotonPriorityQueue::pop()
{
  if(!getSize())
    return NULL;
  else
    {
      PPQueueNode * toRemove = PHeap[1];
      Photon *rv = PHeap[1]->value;
      PHeap[1] = PHeap.back();
      PHeap.pop_back();
      siftdown(1);
      delete toRemove;

      return rv;
    }
}

Photon * PhotonPriorityQueue::top()
{
  return (!getSize()) ? NULL : PHeap[1]->value;
}

//returns sqr distance from location to photon in node.
double PhotonPriorityQueue::distance(PPQueueNode * node)
{
  double dx = node->value->x - location.x;
  double dy = node->value->y - location.y;
  double dz = node->value->z - location.z;
  return SQR(dx)+SQR(dy)+SQR(dz);
}
