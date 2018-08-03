// This may look like C code, but it is really -*- C++ -*-

// SceneTest.cc

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
  int frames;
  string fname;
  double dtdf;
  while(c!='q')
    {
      std::cout << "(q)uit, (f)ile:";
      std::cin >> c;
      if(c=='f')
	{
	  if(sc) delete sc;
	  sc = new Scene();
	  sc->ReadFile();
	  std::cout << "(q)uit, (g)enerate map, (l)oad map, (d)isplay and generate map, (c)reate frames: ";
	  std::cin >> c;
	  switch(c)
	    {
	    case 'c':
	      std::cout << "Number of frames? ";
	      std::cin >> frames;
	      std::cout << "Change in t per frame? ";
	      std::cin >> dtdf;
	      std::cout << "Number of photons for each image? ";
	      std::cin >> nPhotons;
	      std::cout << "filename? (<filename> --> <filename><n>.jpg) ?  ";
	      std::cin >> fname;
	      
	      g_Scene->generateFiles(fname.c_str(),
				     frames,
				     dtdf,
				     nPhotons
				     );
	      break;
	    case 'g':
	      std::cout << "Number of photons? ";
	      std::cin >> nPhotons;
	      g_map = g_Scene->myRenderer->map(nPhotons);
	      std::cout << "Map Generated.\n";
	      std::cout << "(s)ave map / (d)isplay map / (k)dTree map\n";
	      std::cin >> c;
	      switch(c)
		{
		case 'k':
		  g_map->buildTree();
		  sc->draw();
		  glutMainLoop();
		  break;
		case 's': 
		  saveMap(g_map);
		  std::cout << "(q)uit / (d)isplay map\n";
		  std::cin >> c;
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
	      std::cout << "Map loaded.\n";
	      std::cout << "(d)isplay map\n";
	      std::cin >> c;
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
  return 0;
}

PhotonMap * loadMap()
{
  std::cout << "Enter filename to load from: ";
  string fileName;
  std::cin >> fileName;
  ifstream inFile(fileName.c_str());
  if(!inFile)
    {
      std::cerr << "Could not open file to load.\n";
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
  std::cout << "Enter filename to save to: ";
  string fileName;
  std::cin >> fileName;
  ofstream outFile(fileName.c_str());
  if(!outFile)
    {
      std::cerr << "Could not open file to save.\n";
    }
  else
    {
      pmap->out(outFile);
      std::cout << "Photon Map saved to " << fileName <<std::endl;
      outFile.close();
    }
}
