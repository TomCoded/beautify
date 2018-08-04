// This may look like C code, but it is really -*- C++ -*-

// SceneTest.cc

// (C) 2002 Tom White

#include <parallel_pm.h>
#include "Scene/Scene.h"
#include <unistd.h>
#include <GL/glut.h>
#include <netdb.h>
#include <sys/param.h>

void display();

#ifdef PARALLEL
MPI_Datatype MPI_PHOTON;
#endif

extern Scene * g_Scene;
extern PhotonMap * g_map;
bool g_parallel;
bool g_suppressGraphics;

PhotonMap * loadMap(string fileName);
void saveMap(PhotonMap *, string fileName);

int main(int argc, char ** argv) {

#ifdef PARALLEL
  MPI_Init(&argc,&argv);

  {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    char hostname[63];
    gethostname(hostname,63);
    std::cout << "Process " << rank << " running on " 
	 << hostname <<std::endl;
  }
  int nodes;
  MPI_Comm_size(MPI_COMM_WORLD,&nodes);
  int blocklen[3] = { 9, 1, 1 };
  MPI_Aint disp[3] = { 0, (9 * sizeof(float)), (9*sizeof(float)+sizeof(int)) };
  MPI_Datatype types[3] = { MPI_FLOAT, MPI_INT, MPI_SHORT };
  if (MPI_Type_struct(3,blocklen,disp,types,&MPI_PHOTON) != MPI_SUCCESS) {
    std::cerr << "Could not initialize MPI_PHOTON.\n";
    exit(1);
  }
  if (MPI_Type_commit(&MPI_PHOTON) != MPI_SUCCESS) {
    std::cerr << "Could not commit MPI_PHOTON.\n";
    exit(1);
  }
#endif

  int optch;
  int frames=0;
  int startFrame=0;
  string fname;
  string sceneFile;
  int help=0;
  if(argc==0) help=1;
  int nPhotons=0;
  double dtdf;
  bool mapExport, mapImport;
  mapExport=mapImport=g_suppressGraphics=false;
  string mapOutName, mapInName;
  int neighbors=0;
  double minDist=1.0;
  double maxDist=2.0;

  while((optch = getopt(argc, argv, "s:n:N:d:D:f:F:M:m:t:r:pS?")) != -1) {
    switch(optch) {
    case 's': sceneFile = optarg; break;
    case 'n': nPhotons = atoi(optarg); break;
    case 'N': neighbors = atoi(optarg); break;
    case 'd': minDist = atof(optarg); break;
    case 'D': maxDist = atof(optarg); break;
    case 'f': frames = atoi(optarg); break;
    case 'r': startFrame = atoi(optarg); break;
    case 'F': fname = optarg; break;
    case 't': dtdf = atof(optarg); break;
    case 'M': mapExport = true;
      mapOutName = optarg; break;
    case 'm': mapImport = true;
      mapInName = optarg; break;
    case 'p': g_parallel = true; break;
    case 'S': g_suppressGraphics = true; break;
    case '?': help = 1; break;
    };
  };

  if(help) {
    std::cout << "Usage:\n";
    std::cout << argv[0] << "-s <scene> -n <photons> [-N <neigbors>] [-d <minDist>]\n";
    std::cout << "          [-f <frames> -F <framefilebase> -t <dtdf> [-r <start frame>] ]\n";
    std::cout << "          [-[m|M] <mapfile>] [-?] [-S]\n";
    std::cout << "Parameters:   \n";
    std::cout << "  -s <scene>         The name of the Scene file to read.\n";
    std::cout << "  -n <photons>       The number of photons to use for each image.\n";
    std::cout << "  -N <neighbors>     The desired number of photons to use in radiance estimate.\n";
    std::cout << "  -d <minDist>       Photons within this radius of a\n";
    std::cout << "   given location will never be discarded when calculating flux,\n";
    std::cout << "   unless there are <neighbors> or it's default setting photons closer.\n";
    std::cout << "  -D <maxDist>       Photons more distant than maxDist from a point will never\n";
    std::cout << "   be included in the irradiance estimate for that point.\n";
    std::cout << "  -r <start frame>   The number of the frame to resume a job at.\n";
    std::cout << "  -f <frames>        The number of frames to output in batch mode.\n";
    std::cout << "  -F <framefilebase> The base name of frame output jpegs.\n";
    std::cout << "  -t <dtdf>          Change in t per frame\n";
    std::cout << "  -M <mapfile>       Output generated photon map to <mapfile>\n";
    std::cout << "  -m <mapfile>       Import scene's Photon map from <mapfile>\n";
    std::cout << "  -p                 G_Parallel execution using MPI\n";
    std::cout << "  -S                 Suppress graphic output\n";
    std::cout << "  -?                 Output this message\n";
  }

  PhotonMap pMap;

#ifdef PARALLEL
  int rank;
  if (g_parallel) {
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if(!(rank||g_suppressGraphics)) glutInit(&argc,argv);
  }
#else
  if(!g_suppressGraphics)
    glutInit(&argc,argv);
#endif

#if 1	
  int i,j;
  char c='\x00';
  Scene * sc=0;
  //  glutInitDisplayMode(GLUT_DOUBLE|GLUT_DEPTH);
  if(sc) delete sc;
  
  sc = new Scene();

  if(nPhotons) {
    if(mapImport) {
      sc->ReadFile(sceneFile);
      std::cout << "Inputting map from " << mapInName <<std::endl;
      g_map = loadMap(mapInName);
      std::cout << "Map Loaded\n";
    }
    if(frames) {
      sc->ReadFile(sceneFile);
#ifdef PARALLEL      
      if (g_parallel) nPhotons /= nodes;
#endif
      g_Scene->generateFiles(fname.c_str(),
			     startFrame,
			     frames,
			     nPhotons,
			     neighbors,
			     minDist,
			     maxDist
			     );
    } else {
      if(!g_suppressGraphics) {
	//output to screen
	sc->ReadFile(sceneFile);
#ifdef PARALLEL
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	g_Scene->myRenderer->setSeed(rank);
	nPhotons /= nodes;
#endif	
	g_map = g_Scene->myRenderer->map(nPhotons);
#ifdef PARALLEL
	if (g_parallel) {
	  double start = MPI_Wtime();
	  if (rank) {
	    // slave processes
	    int sendsize = g_map->getSize();
	    Photon *tmp = (Photon *)malloc(nPhotons * sizeof(Photon));
	    g_map->getArrMembers(tmp);
	    MPI_Send(tmp,sendsize,MPI_PHOTON,0,rank,MPI_COMM_WORLD);
	    delete tmp;
	  } else {
	    // master process
	    int cnt, totalsize = 0;
	    MPI_Status stat;
	    Photon *tmp = (Photon *)malloc(nPhotons * (nodes - 1) * sizeof(Photon));
	    for(int i = 1; i < nodes; i++) {
	      MPI_Probe(i,MPI_ANY_TAG,MPI_COMM_WORLD,&stat);
	      MPI_Get_count(&stat,MPI_PHOTON,&cnt);
	      MPI_Recv((tmp + totalsize),cnt,MPI_PHOTON,i,MPI_ANY_TAG,MPI_COMM_WORLD,&stat);
	      totalsize += cnt;
	    }
	    for(int i = 0; i < totalsize; i++) {
	      g_map->addPhoton(*(tmp + i));
	    }
	    delete tmp;
	  }
	  double stop = MPI_Wtime();
	  if(!rank)
	    std::cout << "Took " << stop - start << " seconds to create photon map.\n";
	}
#endif
#ifdef PARALLEL
	//	int rank;
	//	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	if(!rank) {
	  g_map->buildTree();
	  g_map->setMinSearch(minDist);
	  if(neighbors)
	    g_map->setNumNeighbors(neighbors);
	  else
	    g_map->setNumNeighbors(g_map->getSize()/10);
	}
#else
	g_map->buildTree();
	g_map->setMinSearch(minDist);
	if(neighbors)
	  g_map->setNumNeighbors(neighbors);
	else
	  g_map->setNumNeighbors(g_map->getSize()/10);
#endif
#ifdef PARALLEL
        if (g_parallel) {
          int rank;
          MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	  sc->draw();
          if(!rank) {
	    std::cout << "Photon Map Size: " << g_map->getSize() <<std::endl;
	    if(!g_suppressGraphics)	    
	      glutMainLoop();
          } 
        }
	else {
#endif
	sc->draw();
	if(!g_suppressGraphics)
	  glutMainLoop();
#ifdef PARALLEL
	}
#endif
      } else {
	sc->ReadFile(sceneFile);
	std::cout << "Graphics Suppression not yet implemented\n";
	exit(1);
      }
    }
    if(mapExport) {
      if(g_map) {
	std::cout << "Outputting map to " << mapOutName <<std::endl;
	saveMap(g_map,mapOutName);
      }
    } 
  }
  if(sc) delete sc;
#ifdef PARALLEL
  return (g_parallel) ? MPI_Finalize() : 0;
#else
  return 0;
#endif
#endif
}

PhotonMap * loadMap(string fileName)
{
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

void saveMap(PhotonMap * pmap, string fileName)
{
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
