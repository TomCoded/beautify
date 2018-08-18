// This may look like C code, but it is really -*- C++ -*-

// SceneTest.cc

// (C) Tom White

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
extern bool g_parallel;
extern bool g_suppressGraphics;

PhotonMap * loadMap(std::string fileName);
void saveMap(PhotonMap *, std::string fileName);

int initMPI();
void printHelp(char ** argv);

int main(int argc, char ** argv) {

#ifdef PARALLEL 
  MPI_Init(&argc,&argv);
  int nodes=initMPI();
#endif
  
  int optch;
  int frames=0;
  int startFrame=0;
  std::string fname("default");
  std::string sceneFile("scenes/sample.dat");
  int help=0;
  if(argc==0) help=1;
  int nPhotons=0;
  double dtdf;
  bool mapExport, mapImport;
  mapExport=mapImport=g_suppressGraphics=false;
  std::string mapOutName, mapInName;
  int neighbors=0;
  double minDist=0.5;
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
    printHelp(argv);
  }

  PhotonMap pMap;
  int rank=0;
#ifdef PARALLEL
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&nodes);
  if(!g_parallel && nodes>1) {
    if(!rank) {
      std::cerr << nodes << " processes started. Turning on parallel option." << std::endl;
    }
    g_parallel = true;
  }
#endif

  //Initialize GLUT
  if(!g_suppressGraphics) {
    if(!(rank)) {
      //fixme: glut chokes if there is not an output here.
      std::cout<<"glutInit()"<<std::endl;
      glutInit(&argc,argv);
      //  glutInitDisplayMode(GLUT_DOUBLE|GLUT_DEPTH);
    }
  }

  int i,j;
  char c='\x00';
  Scene * sc=0;
  sc = new Scene();

  if(nPhotons) {
    sc->ReadFile(sceneFile);
    if(mapImport) {
      std::cout << "Inputting map from " << mapInName <<std::endl;
      g_map = loadMap(mapInName);
      std::cout << "Map Loaded\n";
    }
#ifdef PARALLEL
    if (g_parallel) {
      nPhotons /= nodes;
    }
#endif
  }
  
  sc->willHaveThisManyPhotonsThrownAtIt(nPhotons);
  sc->willNotUseMoreThanThisManyPhotonsPerPixel(neighbors);
  sc->willNeverDiscardPhotonsThisClose(minDist);
  sc->willAlwaysDiscardPhotonsThisFar(maxDist);

  //render to files and/or screen
  if(frames||!g_suppressGraphics) {
    if(frames) {
      sc->willWriteFilesWithBasename(fname.c_str());
      sc->willWriteThisManyFrames(frames);
      sc->willStartOnFrame(startFrame);
    }
    sc->startMainLoop(rank);
  } else {
    std::cerr << "Nothing to do." << std::endl;
  }
  
  if(mapExport) {
    ASSERT(false,"Map Export Not Supported.");
    if(g_map) {
      std::cout << "Outputting map to " << mapOutName <<std::endl;
      saveMap(g_map,mapOutName);
    }
  }
  
  if(sc) delete sc;

#ifdef PARALLEL
  return (g_parallel) ? MPI_Finalize() : 0;
#else
  return 0;
#endif
  
}

int initMPI() {
#ifdef PARALLEL

  int nodes;
  {
    MPI_Comm_size(MPI_COMM_WORLD,&nodes);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    char hostname[63];
    gethostname(hostname,63);
    std::cout << "Process " << rank << " running on " 
	      << hostname <<std::endl;
  }
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
  return nodes;
}

void printHelp(char ** argv) {
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
    std::cout << "                     (must be configured with --enable-mpi during build)\n";
    std::cout << "  -S                 Suppress graphic output\n";
    std::cout << "  -?                 Output this message\n";
    std::cout << "  NOTE: Parallel execution should be started with mpirun";
}

PhotonMap * loadMap(std::string fileName)
{
  std::ifstream inFile(fileName.c_str());
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

void saveMap(PhotonMap * pmap, std::string fileName)
{
  std::ofstream outFile(fileName.c_str());
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
