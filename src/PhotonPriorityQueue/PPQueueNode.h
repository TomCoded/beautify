#ifndef PPQUEUENODE_H_
#define PPQUEUENODE_H_

#include <Photon/Photon.h>

typedef struct PPQN
{
  Photon * value;
  double distance;
} PPQueueNode;

#endif
