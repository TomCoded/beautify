#include <Photon/Photon.h>
#include <Point3Dd.h>
#include "PhotonPriorityQueue.h"

int main(int argc, char *argv[]) {
  PhotonPriorityQueue *q = new PhotonPriorityQueue();
  Photon *temp = new Photon();
  temp->x=2;
  temp->y=0;
  temp->z=0;
  temp->r=1;
  temp->g=0;
  temp->dx=0;
  temp->dy=0;
  temp->dz=-1;
  q->add(temp);
  temp = new Photon();
  temp->g=0;
  temp->dx=0;
  temp->dy=0;
  temp->dz=-1;
  temp->x=-2;
  temp->y=0;
  temp->z=1;
  temp->r=1;
  q->add(temp);
  temp = new Photon();
  temp->g=0;
  temp->dx=0;
  temp->dy=0;
  temp->dz=-1;
  temp->x=3;
  temp->y=4;
  temp->z=-1;
  temp->r=1;
  q->add(temp);
  temp = new Photon();
  temp->g=0;
  temp->dx=0;
  temp->dy=0;
  temp->dz=-1;
  temp->x=0;
  temp->y=-6;
  temp->z=0;
  temp->r=1;
  q->add(temp);
  temp = new Photon();
  temp->g=0;
  temp->dx=0;
  temp->dy=0;
  temp->dz=-1;
  temp->x=-5;
  temp->y=0;
  temp->z=0;
  temp->r=1;
  q->add(temp);
  temp = new Photon();
  temp->g=0;
  temp->dx=0;
  temp->dy=0;
  temp->dz=-1;
  temp->x=8;
  temp->y=3;
  temp->z=-2;
  temp->r=1;
  q->add(temp);
  temp = new Photon();
  temp->g=0;
  temp->dx=0;
  temp->dy=0;
  temp->dz=-1;
  temp->x=0;
  temp->y=0;
  temp->z=-6;
  temp->r=1;
  q->add(temp);
  temp = new Photon();
  temp->g=0;
  temp->dx=0;
  temp->dy=0;
  temp->dz=-1;
  temp->x=-2;
  temp->y=-2;
  temp->z=-2;
  temp->r=1;
  q->add(temp);
  temp = new Photon();
  temp->g=7;
  temp->dx=-1;
  temp->dy=-5;
  temp->dz=-1;
  temp->x=2;
  temp->y=-3;
  temp->z=-4;
  temp->r=1;
  q->add(temp);
  int n = 0;
  while (q->pop());
#if 0
    cout << n << ": (" << temp->x << ',' << temp->y <<
	    ',' << temp->z << ")\n";
  }
#endif
#if 0
  //  q->add(&temp);
  temp.x=-6;
  temp.y=-50;
  temp.z=-1;
  temp.r=1;
  //  q->add(&temp);
  temp.x=5;
  temp.y=50;
  temp.z=1;
  temp.r=1;
  //  q->add(&temp);
  temp.x=4;
  temp.y=-2;
  temp.z=0;
  temp.r=1;
  //  q->add(&temp);
#endif
  return 0;
}
