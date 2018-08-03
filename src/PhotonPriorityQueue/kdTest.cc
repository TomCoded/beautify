// This may look like C code, but it is really -*- C++ -*-

// kdTest.cc

// (C) 2002 Tom White

#include "Scene/Scene.h"
#include <unistd.h>
#include <GL/glut.h>

void display();

extern Scene * g_Scene;
extern PhotonMap * g_map;

PhotonMap * loadMap();
void saveMap(PhotonMap *);

int main(int argc, char ** argv) {
  glutInit(&argc,argv);
  int i,j;
  int nPhotons;
  char c='\x00';
  Scene * sc=0;
  //  glutInitDisplayMode(GLUT_DOUBLE|GLUT_DEPTH);
  PhotonMap pMap;
  //  pMap.initialize(15);
  Photon temp;
  temp.x=2;
  temp.y=0;
  temp.z=0;
  temp.r=1;
  temp.g=0;
  temp.dx=0;
  temp.dy=0;
  temp.dz=-1;
  pMap.addPhoton(temp);
  temp.x=-2;
  temp.y=0;
  temp.z=1;
  temp.r=1;
  pMap.addPhoton(temp);
  temp.x=3;
  temp.y=4;
  temp.z=-1;
  temp.r=1;
  pMap.addPhoton(temp);
  temp.x=0;
  temp.y=-6;
  temp.z=0;
  temp.r=1;
  pMap.addPhoton(temp);
  temp.x=-5;
  temp.y=0;
  temp.z=0;
  temp.r=1;
  //  pMap.addPhoton(temp);
  temp.x=-6;
  temp.y=-50;
  temp.z=-1;
  temp.r=1;
  //  pMap.addPhoton(temp);
  temp.x=5;
  temp.y=50;
  temp.z=1;
  temp.r=1;
  //  pMap.addPhoton(temp);
  temp.x=4;
  temp.y=-2;
  temp.z=0;
  temp.r=1;
  //  pMap.addPhoton(temp);
  temp.x=-7;
  temp.y=-2;
  temp.z=0;
  temp.r=1;
  //  pMap.addPhoton(temp);
  temp.x=0;
  temp.y=0;
  temp.z=0;
  temp.r=1;
  pMap.addPhoton(temp);
  //  pMap.addPhoton(temp);
  pMap.buildTree();
  pMap.outputTree(1);
  //  Point3Dd loc(0,0,0.001);
  //  Point3Dd normal(0,0,1);
  //  cout << pMap.getFluxAt(loc,normal);
#if 0
  while(c!='q')
    {
      cout << "(q)uit, (f)ile:";
      cin >> c;
      if(c=='f')
	{
	  if(sc) delete sc;
	  sc = new Scene();
	  sc->ReadFile();
	  cout << "(q)uit, (g)enerate map, (l)oad map, (d)isplay and generate map: ";
	  cin >> c;
	  switch(c)
	    {
	    case 'g':
	      cout << "Number of photons? ";
	      cin >> nPhotons;
	      g_map = g_Scene->myRenderer->map(nPhotons);
	      cout << "Map Generated.\n";
	      cout << "(s)ave map / (d)isplay map\n";
	      cin >> c;
	      switch(c)
		{
		case 's': 
		  saveMap(g_map);
		  cout << "(q)uit / (d)isplay map\n";
		  cin >> c;
		  if(c=='d')
		    {
		      sc->draw();
		      glutMainLoop();
		    }
		  break;
		case 'd':
		  sc->draw();
		  glutMainLoop();
		}
	      break;
	    case 'l':
	      g_map = loadMap();
	      cout << "Map loaded.\n";
	      cout << "(d)isplay map\n";
	      cin >> c;
	      switch(c)
		{
		case 'd':
		  sc->draw();
		  glutMainLoop();
		  break;
		}
	      break;
	    case 'd':
	      sc->draw();
	      glutMainLoop();
	    }
	}
    }
  if(sc) delete sc;
#endif
  return 0;
}

PhotonMap * loadMap()
{
  cout << "Enter filename to load from: ";
  string fileName;
  cin >> fileName;
  ifstream inFile(fileName.c_str());
  if(!inFile)
    {
      cerr << "Could not open file to load.\n";
      exit(1);
    }
  else
    {
      PhotonMap * myMap = new PhotonMap();
      myMap->in(inFile);
      inFile.close();
      return myMap;
    }
  return 0;
}

void saveMap(PhotonMap * pmap)
{
  cout << "Enter filename to save to: ";
  string fileName;
  cin >> fileName;
  ofstream outFile(fileName.c_str());
  if(!outFile)
    {
      cerr << "Could not open file to save.\n";
    }
  else
    {
      pmap->out(outFile);
      cout << "Photon Map saved to " << fileName << endl;
      outFile.close();
    }
}
