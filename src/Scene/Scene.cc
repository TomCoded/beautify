// This may look like C code, but it is really -*- C++ -*-

// Scene.cc

// (C) 2002 Tom White

#include <allIncludes.h>
#include <iostream>
#include <string>
#include <string.h>
#include <GL/glut.h>
#include <Magick++.h>
#include <PhotonMap/PhotonMap.h>
#include <Material/Material.h>
#include <Material/Participating/Participating.h>
#include <Light/PointLight/DiffusePointLight/DiffusePointLight.h>
#include <Light/SquareLight/SquareLight.h>

//saves volume map to volmap.map
// #define SAVE_VOLMAP

//in SceneTest.cc
void saveMap(PhotonMap * pmap, string fileName);

#ifdef PARALLEL
int g_nFrame;
#endif

#ifdef DIFFUSE
#undef DIFFUSE
#endif

#define MAX_PHOTONS_THROWN 5000000

#define AMBIENT 1
#define DIFFUSE 2
#define SPECULAR 4
#define SHADOWS 8
#define NONREFTRANSPARENCY 16
#define REFLECTION 32

#define LAMBERT AMBIENT | DIFFUSE
#if 0
#define PHONG AMBIENT | DIFFUSE | SPECULAR
#define PHONGS PHONG | SHADOWS
#define PHONGST PHONGS | NONREFTRANSPARENCY
#define PHONGSR PHONGS | REFLECTION
#define PHONGSRT PHONGSR | NONREFTRANSPARENCY
#endif

#define ambientDefault Point3Dd(0,0,0)
#define diffuseDefault Point3Dd(0,0,0)
#define specularDefault Point3Dd(0,0,0)
#define specularExponentDefault 0.0
#define reflectionDefault 0.0
#define transparentDefault 0.0
#define transDefault MakeTranslation(0,0,0);
#define transDefaultInv MakeTranslation(0,0,0);
#define scaleDefault MakeScale(1,1,1);
#define scaleDefaultInv scaleDefault
#define IDENTITY MakeTranslation(0,0,0);

#define MACROcreateShader(s) createShader(s,                            \
                                          matAmbient, matDiffuse, matSpecular,matSpecExp,matReflection, \
                                          matTransparent)

#define shaderDefault MACROcreateShader(LAMBERT);

int antialias;
int g_specModel;
Scene * g_Scene;
PhotonMap * g_map;
#define rayTrace 0
#define photonMap 1

double lastTime;
double curTime;

extern bool g_parallel;
extern bool g_suppressGraphics;

//true iff we need to regenerate pmap between frames.
bool g_dynamicMap;
bool g_regenerate;

char outbuffer[80];

////////////////////
// class Scene //
////////////////////

// A scene 
//

// essentials
//glut display callback function
void display()
{
  if(!g_suppressGraphics)
    glClear(GL_COLOR_BUFFER_BIT);
  
  curTime+=g_Scene->dtdf;

#ifdef DEBUG_BUILD
  std::cout << "dtdf is " << g_Scene->dtdf << std::endl;
  std::cout << "frame time is " << curTime << std::endl;
#endif
  
  if(g_Scene->myRenderer) 
    {
      if(photonMap)
        {
          if(lastTime==curTime)
            {
              if(g_Scene->hasImage)
                {
                  g_Scene->repaint();
                }
              else
                g_Scene->myRenderer->showMap(g_map);
            }
          else
            {
              vector<Surface *> * surfaces = g_Scene->getSurfaces();
              vector<Surface *>::iterator itSurf = surfaces->begin();
              for(;itSurf!=surfaces->end(); itSurf++)
                {
                  (*itSurf)->setTime(curTime);
                }
              //g_Scene->myRenderer->map();
              g_Scene->myRenderer->showMap(g_map);
            }
        }
      else if(rayTrace)
        g_Scene->myRenderer->run();
    }
  if(!g_suppressGraphics) {
    glFlush();
    glutPostRedisplay();
  }
}

//glut keyboard function
void keyboard(unsigned char c, int, int)
{
  switch(c)
    {
    case 'n':
      if(g_Scene->hasImage)
        {
          std::cout << "Advancing to (n)ext frame...\n";
          glutPostRedisplay();
        }
      break;
    case 's':
      if(g_Scene->hasImage)
        {
          std::cout << "Image file name: ";
          string fileName;
          std::cin >> fileName;
          g_Scene->writeImage(fileName.c_str());
        }
      break;
    case 'r':
      if(g_Scene->hasImage)
        {
          std::cout << "Redisplaying Image\n";
          g_Scene->repaint();
        }
      break;
    case 't':
      if(g_Scene->hasImage)
        {
          std::cout << "Smoothing Image\n";
          g_Scene->smoothLogicalImage();
          g_Scene->repaint();
        }
      break;
    case 'q': 
#ifdef PARALLEL
      if(g_parallel) 
        MPI_Finalize();
#endif
      exit(0);
      break;
    default:
      break;
    }
}

//glut reshape function
//The GLUT window resize callback function
void reshape(GLsizei scr_w, GLsizei scr_h)
{
  if((scr_w != g_Scene->width)||(scr_h != g_Scene->height))
    glutReshapeWindow(g_Scene->width,g_Scene->height);
  else
    {
      glMatrixMode(GL_PROJECTION);
      glViewport(0,0,scr_w,scr_h);
      glLoadIdentity();
      glOrtho(-0.5,0.5,-0.5,0.5,0.25,2.0);
      glMatrixMode(GL_MODELVIEW);
      glutPostRedisplay();
    }
}

//Updates all transformations in the scene to 
//be generated based on the current time
void Scene::setTime(double t) {
  vector<Surface *>::iterator itSurf;
  vector<Camera *>::iterator itCam;

  curTime=t;

  for(itSurf=surfaces->begin();
      itSurf!=surfaces->end();
      itSurf++)
    (*itSurf)->setTime(t);
  for(itCam=cameras->begin();
      itCam!=cameras->end();
      itCam++)
    (*itCam)->setTime(t);
}

// batch video file generator 
void Scene::generateFiles(const char * filename, 
                          int startFrame,
                          int numFrames, 
                          double dfdt,
                          int photons,
                          int neighbors,
                          double minDist,
                          double maxDist
                          )
{
  logicalRender=true;
  char szFile[63];
  char szTail[10];
  strcpy(szFile,filename);
  int nBaseLen = strlen(filename);
  char * nTail = &szFile[nBaseLen];

  setTime(startFrame*dfdt);

#ifdef PARALLEL
  int rank, nodes;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&nodes);
  if(!rank) {
    std::cout <<std::endl;
    std::cout << "frame  proc  procs pMap Size  pMap dist render   pMap create kdTree\n";
    std::cout << "-----  ----  ----- ---------  --------- ------- ----------- ------\n";
  }
  //  photons /= nodes;
#endif
  paintingFromLogical=false;

  for(int nFrame=startFrame; nFrame<numFrames+startFrame; nFrame++)
    {
      //      MPI_Barrier(MPI_COMM_WORLD);
      //paint the map into the array
      //      paintingFromLogical=false;

#ifdef PARALLEL
      g_nFrame = nFrame;
      myRenderer->setSeed(rank);
#else
      std::cout << "Creating photon map for frame " << nFrame <<std::endl << flush;
#endif


      if(g_dynamicMap || (nFrame == startFrame) ) {
        while( (g_map = g_Scene->myRenderer->map(photons) )->getSize() < 10) {
          //give it a blank image
#ifdef PARALLEL	  
          if (!rank)	
#endif
            std::cerr << "Photon Map of " << g_map->getSize() 
                      << " photons Too Small to Render frame" << nFrame 
                      << "; ";
          map_too_small=true;
          break;
          //	      MPI_Abort(MPI_COMM_WORLD,1);
        }
      }

#ifdef PARALLEL
      double frame_start = MPI_Wtime();
      if (g_parallel) {
        double start = MPI_Wtime();

        if(g_dynamicMap || (nFrame == startFrame) ) {
          if(!map_too_small) {
            g_regenerate = true;
	    
            g_map->gatherPhotons(photons);
            MPI_Barrier(MPI_COMM_WORLD);
            //	  myRenderer->getVolMap()->gatherPhotons(photons);
            int bufSize = myRenderer->getVolMap()->getSize()*(nodes+2);
            myRenderer->getVolMap()->gatherPhotons(bufSize > photons ? bufSize : photons);
#define TAG_HANDSHAKE 6000

            double stop = MPI_Wtime();

            if(!rank) {
              sprintf(outbuffer,"%5d  %4d  %5d %9d  %9f %6f  ",
                      g_nFrame, rank, nodes, g_map->getSize(),
                      0.0, 0.0, 
                      stop-start);
              //	    std::cout << "Took " << stop - start << " seconds to create photon map.\n";
              start = MPI_Wtime();

              g_map->buildTree();
              myRenderer->getVolMap()->buildTree();

              stop = MPI_Wtime();
              sprintf(outbuffer + strlen(outbuffer),"%6f",stop-start);
              printf("%s\n",outbuffer);
              //	    std::cout << "Took " << stop - start << " seconds to build kdTree.\n";
              if(minDist) {
                g_map->setMinSearch(minDist);
                myRenderer->getVolMap()->setMinSearch(minDist);
              }
              if(maxDist) {
                g_map->setMaxDist(maxDist);
                myRenderer->getVolMap()->setMaxDist(maxDist);
              }
              if(neighbors) {
                g_map->setNumNeighbors(neighbors);
                myRenderer->getVolMap()->setNumNeighbors(neighbors);
              }
              else {
                g_map->setNumNeighbors(g_map->getSize()/15);
                myRenderer->getVolMap()->setNumNeighbors(g_map->getSize()/15);
              }
            }
          } 
        } else {
          g_regenerate = false;
        }
        start = MPI_Wtime();
	
        draw();

        double stop = MPI_Wtime();
        //	if(!rank)
        //	  std::cout << "Took " << stop - start << " seconds to render image.\n";
      }
      
      if(!rank) {
        sprintf(szTail,"%d.jpg\x00",nFrame);
        strncpy(nTail,szTail,strlen(szTail)+1);
        writeImage(szFile);
      }
      setTime(curTime+dfdt);
      //why was this here?
      //	logicalRender=false;

      //synchronize between frames.
      MPI_Barrier(MPI_COMM_WORLD);
      double frame_stop = MPI_Wtime();
      if(!rank)
        std::cout << "Frame " << nFrame << " Done in " <<
          frame_stop - frame_start << " seconds.\n";
#else
      std::cout << "Rendering frame " << nFrame <<std::endl;
      //FIXME: Use a better algorithm for numneighbors
      g_map->setNumNeighbors(g_map->getSize()/10);

      myRenderer->showMap(g_map);

      sprintf(szTail,"%d.jpg\x00",nFrame);
      strncpy(nTail,szTail,strlen(szTail)+1);
      writeImage(szFile);

      setTime(curTime+dfdt);

      logicalRender=false;
#endif
    }
  //  MPI_Barrier(MPI_COMM_WORLD);
  //  std::cout <<std::endl << rank << " post-draw() waiting at barrier.\n" << flush;
  //inter-frame barrier
#ifdef PARALLEL
  MPI_Barrier(MPI_COMM_WORLD);
#endif
}

// default constructor
Scene::Scene():
  currentCamera(0),
  width(300),
  height(300),
  hasImage(0),
  dtdf(0),
  logicalImage(0),
  logicalRender(false),
  paintingFromLogical(false)
{
  lights = new vector<Light *>();
  surfaces = new vector<Surface *>();
  cameras = new vector<Camera *>();
  materials = new vector<Material *>();
  g_specModel=HALFWAY;
  myRenderer = new Renderer(this);
  g_Scene=this;
  Magick::InitializeMagick("/usr/lib");
#ifdef PARALLEL
  if (g_parallel) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if(!(rank||g_suppressGraphics)) {
      glutInitWindowSize(width,height);
      glutInitWindowPosition(200,200);
      glutCreateWindow("The Scene:");

      glMatrixMode(GL_MODELVIEW);
      gluLookAt(0.5,0.5,1.0, //eye
                0.5,0.5,0, //lookAt
                0,1.0,0); //up

      glClearColor(0.0,1.0,0.0, 0.0f); //Green Background
      glColor3f(1.0f,0.0f,0.0f); //drawing color
      glutDisplayFunc(display);
      glutKeyboardFunc(keyboard);
      glutReshapeFunc(reshape);
    }
  }
#else
  if(!g_suppressGraphics) {
    glutInitWindowSize(width,height);
    glutInitWindowPosition(200,200);
    glutCreateWindow("The Scene:");

    glMatrixMode(GL_MODELVIEW);
    gluLookAt(0.5,0.5,1.0, //eye
              0.5,0.5,0, //lookAt
              0,1.0,0); //up

    glClearColor(0.0,1.0,0.0, 0.0f); //Green Background
    glColor3f(1.0f,0.0f,0.0f); //drawing color
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
  }
#endif
}

// destructor
Scene::~Scene() 
{
  //  if(g_map)
  //    delete g_map;
  if(hasImage)
    delete logicalImage;

  vector<Light *>::iterator itDestroyer = lights->begin();
  while( itDestroyer != lights->end() )
    { //destroy all light sources in the vector
      if(*itDestroyer) delete (*itDestroyer);
      itDestroyer++;
    }
  delete lights;

  vector<Surface *>::iterator itSDestroyer = surfaces->begin();
  while( itSDestroyer != surfaces->end() )
    { //destroy all surfaces in the vector
      delete (*itSDestroyer);
      itSDestroyer++;
    }
  delete surfaces;

  vector<Material *>::iterator itMatDestroyer = materials->begin();
  while( itMatDestroyer != materials->end() )
    { //destroy all Materials in the vector
      delete (*itMatDestroyer);
      itMatDestroyer++;
    }
  delete materials;

  vector<Camera *>::iterator itCDestroyer = cameras->begin();
  while(itCDestroyer != cameras->end() )
    {
      delete (*itCDestroyer);
      itCDestroyer++;
    }
  if(myRenderer) delete myRenderer;
}

// Scene features
void Scene::ReadFile(string fileName) {
  Point3Dd currentPoint3Dd;
  Point3Df currentPoint3Df;
  double currentDouble;
  Point3Dd totalTranslation;
  Point3Dd totalScale;
  double totalXRotate;
  double totalYRotate;
  double totalZRotate;
  Transform4Dd currentTranslation=transDefault
    Transform4Dd currentTranslationInverse=transDefaultInv
    Transform4Dd currentScale=scaleDefault
    Transform4Dd currentScaleInverse=scaleDefaultInv
    Transform4Dd currentXRotate=IDENTITY
    Transform4Dd currentYRotate=IDENTITY
    Transform4Dd currentZRotate=IDENTITY
    Transform4Dd userTransform=IDENTITY
    Transform4Dd userTransformInverse=IDENTITY
    Transform4Dd rotTransform=IDENTITY
    Transform4Dd rotTransformInverse=IDENTITY
    Transform4Dd currentWTLTransform=IDENTITY
    Transform4Dd currentLTWTransform=IDENTITY
    Transform4Dd currentLTWNTransform=IDENTITY

    FunTransform4Dd * ftCurrentWTLTransform
    = new FunTransform4Dd(1,0,0,0,
                          0,1,0,0,
                          0,0,1,0,
                          0,0,0.0,1
                          );
  FunTransform4Dd * ftCurrentLTWTransform
    = new FunTransform4Dd(1,0.0,0,0,
                          0,1,0,0,
                          0,0,1,0,
                          0,0,0,1
                          );
  FunTransform4Dd * ftCurrentLTWNTransform
    = new FunTransform4Dd(1.0,0,0,0,
                          0,1,0,0,
                          0,0,1,0,
                          0,0,0,1
                          );

  bool usingFunTransform=false;
  
  Point3Dd matAmbient = ambientDefault;
  Point3Dd matDiffuse = diffuseDefault;
  Point3Dd matSpecular = specularDefault;
  double matSpecExp = specularExponentDefault;
  double matReflection = reflectionDefault;
  double matTransparent = transparentDefault;

  int shaderID = LAMBERT;

  //  DirLight * tempDirLight;
  PointLight * tempPointLight;
  Shader * tempShader;

  int numSurfaces = -1;
  int numLights = -1;

  char c; //scratch

  // create an input filestream
  ifstream inFile(fileName.c_str());

  // check for success of stream constructor
  if(!inFile) { // could not open file
    std::cerr << "Could not open file: " << fileName << " exiting...";
    exit(1);
  }

  // read in contents
  string keyword;

  while(!inFile.eof()) {
    inFile >> keyword;

    if(keyword == "include") {
      string ifile;
      inFile >> ifile;
      ReadFile(ifile);
    }
    else if(keyword == "numSurfaces") {
      inFile >> numSurfaces;
    }
    else if(keyword == "numLights") {
      inFile >> numLights;
    }
    else if(keyword == "windowDims") {
      inFile >> c;
      if(c!='(') BADFORMAT("( not in windowDims (,)")
                   inFile >> height;
      inFile >> c;
      if(c!=',') BADFORMAT(", not in windowDims (,)")
                   inFile >> width;
      inFile >> c;
      if(c!=')') BADFORMAT(") not in windowDims (,)")
                   }
    else if(keyword == "sceneAmbient") {
      inFile >> ambient;
    }
    else if(keyword == "antialias") {
      int antiAlias;
      inFile >> antiAlias;
      antialias = antiAlias;
      //      std::cout << "antialias = " << antiAlias <<std::endl;
    }
    /*
      else if(keyword == "fast") {
      int fast;
      inFile >> fast;
      // std::cout << "fast = " << fast <<std::endl;
      }
    */
    else if(keyword == "camera") {
      Camera * cam = new Camera();
      cam->in(inFile);
      addCamera(cam);
    }
    else if(keyword == "funcam") {
      Camera * cam = new Camera();
      cam->funIn(inFile);
      addCamera(cam);
    }
#if 0
    else if(keyword == "pointLight") {
      tempPointLight = new PointLight();
      inFile >> (*tempPointLight);
      addLight(tempPointLight);
    }
    else if(keyword == "spotLight") {
      std::cout << "spotLight ";
      std::cout << "skipping parameters..." <<std::endl;
      skipDescription(inFile);
    }
#endif
    else if(keyword == "DiffusePointLight")
      {
        DiffusePointLight * dpLight
          = new DiffusePointLight();
        inFile >> (*dpLight);
        addLight(dpLight);
      }
    else if(keyword == "DirLight") {
      DirLight * tempDirLight = new DirLight();
      inFile >> (*tempDirLight);
      addLight(tempDirLight);
    }
    else if(keyword == "SquareLight") {
      SquareLight * mySquareLight 
        = new SquareLight();
      inFile >> (*mySquareLight);
      addLight(mySquareLight);
    }
    else if(keyword == "Material")
      {
        Material * myMat
          = new Material();
        inFile >> (*myMat);
        addMaterial(myMat);
        lastMaterial = myMat;
      }
    else if(keyword == "Participating")
      { //for the money, folks.
        Participating * myPart
          = new Participating();
        myPart->in(inFile);
        addMaterial(myPart);
        lastMaterial = myPart;
      }
    //temporarily remove shaders
#if 0 
    else if(keyword == "ambient") {
      shaderID = AMBIENT;
    }
    else if(keyword == "lambert") {
      //keep a record of what shader we should use when 
      //making new objects
      shaderID = LAMBERT;
    }
    else if(keyword == "phong") {
      shaderID = PHONG;
    }
    else if(keyword == "phongS") {
      shaderID = PHONGS;
    }
    else if(keyword == "phongSR") {
      shaderID = PHONGSR;
    }
    else if(keyword == "phongST") {
      shaderID = PHONGST;
    }
    else if(keyword == "phongSRT") {
      shaderID = PHONGSRT;
    }
    else if(keyword == "phongSRF") {
      std::cout << "Current shader is phongSRF" <<std::endl;
      std::cerr << " SHADER UNIMPLEMENTED";
    }
#endif
    //temporarily remove transforms

    else if(keyword == "identity") {
      // make identity transform
      userTransform = IDENTITY
        userTransformInverse = IDENTITY
        //non-legacy stuff

        usingFunTransform=false;

      rotTransform = IDENTITY
        rotTransformInverse = IDENTITY

        currentWTLTransform=IDENTITY
        currentLTWTransform=IDENTITY
        currentLTWNTransform=IDENTITY

        //      FunTransform4Dd * ftCurrentWTLTransform
        ftCurrentWTLTransform
        = new FunTransform4Dd(1.0,0,0,0,
                              0,1,0,0,
                              0,0,1,0,
                              0,0,0,1
                              );
      //      FunTransform4Dd * ftCurrentLTWTransform
      ftCurrentLTWTransform
        = new FunTransform4Dd(1.0,0,0,0,
                              0,1,0,0,
                              0,0,1,0,
                              0,0,0,1
                              );
      //      FunTransform4Dd * ftCurrentLTWNTransform
      ftCurrentLTWNTransform
        = new FunTransform4Dd(1.0,0,0,0,
                              0,1,0,0,
                              0,0,1,0,
                              0,0,0,1
                              );
    }
    else if(keyword == "translate") {
      Point3Dd trans;
      inFile >> trans;

      //set total transforms
      currentLTWTransform=
        MakeTranslation(trans)*currentLTWTransform;

      currentWTLTransform=
        currentWTLTransform*MakeTranslation(trans*-1);

      //normals are not transformed under translation.

      //do legacy stuff

      totalTranslation+=trans;
      currentTranslation =
        MakeTranslation(totalTranslation);
	
      currentTranslationInverse = 
        MakeTranslation(totalTranslation*-1);
    }
    else if(keyword == "scale") {
      Point3Dd scale;
      inFile >> scale;

      //set total transforms
      currentLTWTransform=
        MakeScale(scale)*currentLTWTransform;

      currentWTLTransform=
        currentWTLTransform*MakeScale(1/scale.x,
                                      1/scale.y,
                                      1/scale.z
                                      );

      currentLTWNTransform=
        MakeScale(scale)*currentLTWTransform;
    }
    else if(keyword == "xRotate") {
      double angle;
      inFile >> angle;

      //set total transforms
      currentLTWTransform=
        MakeXRotation(angle)*currentLTWTransform;
      
      currentWTLTransform=
        currentWTLTransform*MakeXRotation(-angle);

      currentLTWNTransform=
        MakeXRotation(angle)*currentLTWNTransform;

    }
    else if(keyword == "yRotate") {
      double angle;
      inFile >> angle;
      //set total transforms
      currentLTWTransform=
        MakeYRotation(angle)*currentLTWTransform;
      
      currentWTLTransform=
        currentWTLTransform*MakeYRotation(-angle);

      currentLTWNTransform=
        MakeYRotation(angle)*currentLTWNTransform;
    }
    else if(keyword == "zRotate") {
      double angle;
      inFile >> angle;
      //set total transforms
      currentLTWTransform=
        MakeZRotation(angle)*currentLTWTransform;
      
      currentWTLTransform=
        currentWTLTransform*MakeZRotation(-angle);

      currentLTWNTransform=
        MakeZRotation(angle)*currentLTWNTransform;
    }
    else if(keyword == "rotate") {
      char ch; double a; Point3Dd d;
      string formatErr("Bad format for rotation");
      inFile >> ch;
      FORMATTEST(c,'(',formatErr)

        inFile >> a;
      inFile >> ch;
      if(ch!=',')
        FORMATTEST(c,',',formatErr)

          inFile >> d;
      inFile >> ch;
      FORMATTEST(c,')',formatErr)

        double c = cos(a);
      double s = sin(a);
      rotTransform =
        Transform4Dd(c + d.x*d.x*(1-c)  , (1-c)*d.y*d.x-s*d.z,
                     (1-c)*d.z*d.x + s*d.y, 0                  ,
                     (1-c)*d.x*d.y+s*d.z  , c+(1-c)*d.y*d.y    ,
                     (1-c)*d.z*d.y-s*d.x  , 0                  ,
                     (1-c)*d.x*d.z-s*d.y  , (1-c)*d.y*d.z+s*d.x,
                     c+(1-c)*d.z*d.z      , 0                  ,
                     0, 0, 0, 1
                     );
      d = d*-1;
      a = -a;
      c = cos(a);
      s = sin(a);
      rotTransformInverse =
        Transform4Dd(c + (1 - c)*d.x*d.x  , (1-c)*d.y*d.x-s*d.z,
                     (1-c)*d.z*d.x + s*d.y, 0                  ,
                     (1-c)*d.x*d.y+s*d.z  , c+(1-c)*d.y*d.y    ,
                     (1-c)*d.z*d.y-s*d.x  , 0                  ,
                     (1-c)*d.x*d.z-s*d.y  , (1-c)*d.y*d.z+s*d.x,
                     c+(1-c)*d.z*d.z      , 0                  ,
                     0, 0, 0, 1
                     );
      // make a transform from angle and direction
      currentLTWTransform=
        rotTransform*currentLTWTransform;

      currentWTLTransform=
        currentWTLTransform*rotTransform;

      currentLTWNTransform=
        rotTransform*currentLTWNTransform;
    }
    else if(keyword =="dtdf"){
      inFile >> dtdf;
    }
    else if(keyword =="funTransform") {
      FunTransform4Dd * lFunTransform
        = new FunTransform4Dd;
      inFile >> *lFunTransform;
      //ftCurrentLTWTransform = lFunTransform;
      (*ftCurrentLTWTransform)
        *=
      	(*lFunTransform);
      usingFunTransform=true;
      g_dynamicMap=true;
    }
    else if(keyword =="funTransformInverse") {
      FunTransform4Dd * lFunTransform
        = new FunTransform4Dd();
      FunTransform4Dd * old = ftCurrentWTLTransform;
      inFile >> *lFunTransform;
      ftCurrentWTLTransform=lFunTransform;
      (*ftCurrentWTLTransform)
        *=
        (*old);
      usingFunTransform=true;
      g_dynamicMap=true;
    }
    else if(keyword =="funTransformNormals") {
      FunTransform4Dd * lFunTransform
        = new FunTransform4Dd();
      inFile >> *lFunTransform;
      ftCurrentLTWNTransform=
        lFunTransform;
      usingFunTransform=true;
      g_dynamicMap=true;
    }
    else if(keyword == "transform") {
      Transform4Dd lUserTransform;
      inFile >> lUserTransform;
      currentLTWTransform=
        lUserTransform*currentLTWTransform;
    }
    else if(keyword == "invtransform") {
      Transform4Dd lUserTransform;
      std::cin >> lUserTransform;
      currentWTLTransform=
        lUserTransform*currentWTLTransform;
    }
    else if(keyword == "invtransformNormals") {
      Transform4Dd lUserTransform;
      std::cin >> lUserTransform;
      currentLTWNTransform=
        lUserTransform*currentLTWNTransform;
    }
    else if(keyword == "ellipsoid") {
      if(usingFunTransform)
        addSurface(new Surface(new Sphere(),
                               lastMaterial,
                               ftCurrentLTWTransform,
                               ftCurrentWTLTransform,
                               ftCurrentLTWNTransform
                               )
                   );
      else
        addSurface(new Surface(new Sphere(),
                               lastMaterial,
                               currentLTWTransform,
                               currentWTLTransform,
                               currentLTWNTransform
                               )
                   );
    }
    else if(keyword == "plane") {
      if(usingFunTransform)
        addSurface(new Surface(new Plane(),
                               lastMaterial,
                               ftCurrentLTWTransform,
                               ftCurrentWTLTransform,
                               ftCurrentLTWNTransform
                               )
                   );
      else
        addSurface(new Surface(new Plane(),
                               lastMaterial,
                               currentLTWTransform,
                               currentWTLTransform,
                               currentLTWNTransform
                               )
                   );
    }
    else if(keyword == "taperedCyl") {
      double s;
      inFile >> s;
      if(usingFunTransform)
        addSurface(new Surface(new TaperedCyl(s),
                               lastMaterial,
                               ftCurrentLTWTransform,
                               ftCurrentWTLTransform,
                               ftCurrentLTWNTransform
                               )
                   );
      else
        addSurface(new Surface(new TaperedCyl(s),
                               lastMaterial,
                               currentWTLTransform,
                               currentLTWTransform,
                               currentLTWNTransform
                               )
                   );
    }
    else if(keyword.substr(0,2) == "//" ||
            keyword.substr(0,1) == "#") { 
      // If keyword starts with '//' or '#', it's a comment
      inFile.ignore(1000,'\n'); // skip up to and including next end-of-line
    }
    else {
      std::cerr << "Unrecognized keyword: " << keyword << "  Exiting..." <<std::endl;
      exit(1);
    }
  }

  inFile.close();
#ifdef PARALLEL
  int rank=0;
  if(g_parallel) {
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  }
  if(!rank)
#endif
    {
      if(logicalImage)
        delete logicalImage;
      logicalImage = new double[3*(height*width)];
      for(int i=0; i<3*height*width;) {
        logicalImage[i++]=1.0;
        logicalImage[i++]=0.0;
        logicalImage[i++]=0.0;
      }
    }
}

//sets a certain pixel in the graphics window to color color.
void Scene::putPixel(int x, int y, Point3Dd color)
{
  putPixel(x,y,color.x,color.y,color.z);
}

//precondition: window size has been set
void Scene::putPixel(int x, int y, double r, double g, double b)
{
  if(r>1) r=1;
  if(g>1) g=1;
  if(b>1) b=1;
#ifdef PARALLEL
  if(!doingLocalPart) {
#endif
    if(logicalImage&&!paintingFromLogical)
      {
        int nPlace = (y*width+x)*3;
        logicalImage[nPlace++]=r;
        logicalImage[nPlace++]=g;
        logicalImage[nPlace]=b;
        hasImage=true;
      }
    //if we're rendering to the screen
    if(!logicalRender)
      {
        glPointSize(1.5);
        double dx = (x/(double)width);
        double dy = (y/(double)height);
        glColor3f(r,g,b);
        glBegin(GL_POINTS);
        glVertex3d(dx,dy,0.0);
        glEnd();
      }
#ifdef PARALLEL
  }
  else {
    int nPlace = (y*width+x)*3 - (localLogicalStart*3);
    localLogical[nPlace++]=r;
    localLogical[nPlace++]=g;
    localLogical[nPlace]=b;
  }
#endif
}

//repaints scene from logical image
void Scene::repaint()
{
  if(hasImage&&logicalImage)
    {
      int nPos=0;
      paintingFromLogical=true;
      for(int y=0; y<height; y++)
        for(int x=0; x<width; x++)
          {
            putPixel(x,
                     y,
                     logicalImage[nPos],
                     logicalImage[nPos+1],
                     logicalImage[nPos+2]
                     );
            nPos+=3;
          }
    }
}

//smooths the logical image
void Scene::smoothLogicalImage()
{
  if(hasImage&&logicalImage) {
    int nPos=1+width*3;
    paintingFromLogical=true;
    for(int y=1; y<height-1; y++)
      for(int x=1; x<width-1; x++)
        {
          for(int n=0; n<3; n++) {
            logicalImage[nPos] = 
              logicalImage[nPos]*2 +
              logicalImage[nPos + 3] +
              logicalImage[nPos - 3] +
              logicalImage[nPos - (width * 3)] +
              logicalImage[nPos + (width * 3)];
            logicalImage[nPos] = logicalImage[nPos] / 6.0;
            nPos++;
          }
        }
  }
}

//Writes the image to a file
void Scene::writeImage(const char * fileName)
{
  char dims[32];
  sprintf(dims,"%dx%d\x00",width,height);
  //  Image img(dims,"white");
  Magick::Image img (Magick::Geometry(width,height),"white" );
  int nPos=0;
  for(int y=0; y<height; y++)
    for(int x=0; x<width; x++)
      {
        img.pixelColor(x,
                       y,
                       Magick::ColorRGB(logicalImage[nPos],
                                        logicalImage[nPos+1],
                                        logicalImage[nPos+2]
                                        )
                       );
        nPos+=3;
      }

  //This keeps ffmpeg from freezing on blank frames
  //in videos we later create
  img.pixelColor(0,0,Magick::ColorRGB(1.0,1.0,5.0));
  img.pixelColor(1,1,Magick::ColorRGB(1.0,1.0,5.0));
  img.pixelColor(2,1,Magick::ColorRGB(1.0,1.0,5.0));
  img.pixelColor(1,2,Magick::ColorRGB(1.0,1.0,5.0));
  img.pixelColor(2,2,Magick::ColorRGB(1.0,1.0,5.0));

  img.flip();
  img.write(fileName);
}

void Scene::setWindowSize(int x, int y)
{
  width=x;
  height=y;
  if(!g_suppressGraphics)
    glutReshapeWindow(x,y);
  if(hasImage&&logicalImage)
    delete logicalImage;
  logicalImage = new double[width*height*3];
  hasImage=true;
}

int Scene::getWindowWidth()
{ return width; }

int Scene::getWindowHeight()
{ return height; }

void Scene::draw()
{
  myRenderer->ambient=ambient;
  if(photonMap&&(!g_map)) {
    g_map = g_Scene->myRenderer->map();
  }
#ifdef PARALLEL
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Barrier(MPI_COMM_WORLD);
  if(g_parallel) {
    drawParallel();
  }
  if(!(rank||g_suppressGraphics)) {
    glutReshapeWindow(width,height);
    glutPostRedisplay();
  } else if(!rank) 
    display();
#endif
}

#ifdef PARALLEL
void Scene::drawParallel() {
  int nodes, rank;
  double start, stop; //for timings
  MPI_Comm_size(MPI_COMM_WORLD,&nodes);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  int totalSize = getWindowHeight()*getWindowWidth();
  int task = rank;

  //allocate space for a row of pixels.
  localLogicalSize = getWindowWidth();
  //totalSize / nodes;
  //each process starts off with row equal to its rank
  localLogicalStart = rank * localLogicalSize;
  localLogical = new double[3*localLogicalSize];
  doingLocalPart = true;

  //distribute kdTree

  if(!map_too_small) {
    start = MPI_Wtime();

    //only do this if the pmap has been generated this frame
    if(g_regenerate) {
      g_map->distributeTree();
      myRenderer->getVolMap()->distributeTree();
#ifdef SAVE_VOLMAP
      if(!rank) {
        saveMap(myRenderer->getVolMap(),"volmap.map");
      }
#endif
    }

    stop = MPI_Wtime();
    //    printf("%s",outbuffer);
    sprintf(outbuffer,"%5d  %4d  %5d %9d  %9f ",
            g_nFrame, rank, nodes, g_map->getSize(),
            stop - start
            );

    //now actually render
    start = MPI_Wtime();
    MPI_Status status;
    bool done=false;

    if(rank) {
      while(!done) {
        //we are a child process.  Have default task.  Run task.
	
        //now render results
        myRenderer->showMap(g_map,
                            localLogicalStart,
                            localLogicalSize
                            );

        //now send data to master
        MPI_Send(localLogical,localLogicalSize*3,MPI_DOUBLE,
                 0, task, MPI_COMM_WORLD);

        //query master to see if there are jobs left
        int jobs;
        MPI_Send(&jobs,1,MPI_INT,0,TAG_HANDSHAKE,MPI_COMM_WORLD);
        MPI_Recv(&task,1,MPI_INT,
                 0,MPI_ANY_TAG,
                 MPI_COMM_WORLD,&status);
        if(task==-1) {
          done=true;
        }
        else {
          localLogicalStart = task*localLogicalSize;
        }
      }
      MPI_Barrier(MPI_COMM_WORLD);
    } else {
      //we are parent process
      
      //allocate space for bag of tasks and initialize
      int bag = nodes-1; //the number of tasks outstanding.
      int bagReturned = 0; //the number of tasks completed.
      //      bool * nodeArray = new bool[nodes];
      int doneProcs = 0;

      //now go into our loop
      int flag; 
      //place in our own task
      int taskPlace=localLogicalStart;
      bool done2 = false;
      while((doneProcs != nodes)||(!done2)||(bagReturned<(getWindowHeight()-1))) {
        static int lastBag=0;
        static int progressMeter=0;
        if(lastBag!=bagReturned) {
          lastBag = bagReturned;
        }
        MPI_Iprobe(MPI_ANY_SOURCE,
                   MPI_ANY_TAG,
                   MPI_COMM_WORLD,
                   &flag,
                   &status
                   );
        //handle request from child process
        if(flag) {
          int tag = status.MPI_TAG;
          int child = status.MPI_SOURCE;
          if(tag==TAG_HANDSHAKE) {
            //request for data from child
            int size;
            //	    std::cout << "Receiving request from " << child << " for new task.\n";
            int junk;
            MPI_Recv(&junk,1,MPI_INT,child,TAG_HANDSHAKE,MPI_COMM_WORLD,&status);
            //	    std::cout << "Received request...\n";
            junk=-1;
            if(bag>=(getWindowHeight()-1)) {
              done=true;
            }
            if(!done) {
              bag++;
              MPI_Send(&bag,1,MPI_INT,child,0,MPI_COMM_WORLD);
            } else {
              int junk=-1;
              MPI_Send(&junk,1,MPI_INT,child,0,MPI_COMM_WORLD);
              doneProcs++;
            }
          } else {
            //child sent data; dump it into buffer
            progressMeter++;
            if(!(progressMeter % (height/20)))
              std::cout << "|" << flush;
            MPI_Recv(&logicalImage[tag*3*getWindowWidth()],
                     localLogicalSize*3, MPI_DOUBLE,
                     child,tag,MPI_COMM_WORLD,&status);
            bagReturned++;
            //child will poll for new task; next MPI_Iprobe will
            //	detect it
          }
        } else {
          //run a portion of our own task.
          if(!done2) {
            //	    std::cout << "Master process doing pixel " << taskPlace
            //	    <<std::endl;
            myRenderer->showMap(g_map,
                                taskPlace++,
                                1
                                );
            if(taskPlace%width == 0) {
              //	      std::cout << "taskplace is " << taskPlace <<std::endl;
              //	      std::cout << "task " << task << " finished by master\n";
              //now copy stuff over to logicalImage from local
              //	      int done = task*3*width+localLogicalSize*3;
              progressMeter++;
              if(!(progressMeter % (height/20)))
                std::cout << "|" << flush;

              for(int i=0; i<localLogicalSize*3; i++) {
                logicalImage[i+localLogicalStart*3]
                  = localLogical[i];
              }
              bag++;
              task=bag;
              bagReturned++;
              if(bag>=(getWindowHeight()-1)) {
                done=true;
                done2=true;
                doneProcs++;
              }
              else {
                //		std::cout << "task " << task << " to be worked on by master\n";
                localLogicalStart = task * localLogicalSize;
                taskPlace = getWindowWidth()*task+1;
              }
            }
          }
        }
      }
      MPI_Barrier(MPI_COMM_WORLD);
      flag=0;
    }
    stop = MPI_Wtime();
    sprintf(outbuffer + strlen(outbuffer), "%6f\n",stop-start);
  } 
  if(!rank) {
    std::cout <<std::endl;
    std::cerr << "Volumetric Photon Map is size " << myRenderer->getVolMap()->getSize() <<std::endl;

  }
  // Don't think we need this...
  //  MPI_Barrier(MPI_COMM_WORLD);

  printf("%s",outbuffer);
  //now gather information again.  Everything has same amount of data,
  //use gather.
  start = MPI_Wtime();

  //Now we have all the data we need.  Delete local logical buffer.
  delete [] localLogical;
  doingLocalPart = false;
  MPI_Barrier(MPI_COMM_WORLD);
  stop = MPI_Wtime();
  if(!rank)
    printf("%d %d %d Gather Required: %f s\n",g_nFrame,rank,nodes,stop-start);
  //      std::cout << "Took " << stop - start << " seconds to gather pixels.\n";
  hasImage = true;

}
#endif

//returns a pointer to a vector of pointers to all lights in the scene
vector<Light *> * Scene::getLights()
{
  return lights;
}

//returns a pointer to a vector of pointers to all surfaces in the scene
vector<Surface *> * Scene::getSurfaces()
{
  return surfaces;
}

//returns a pointer to the current camera
Camera * Scene::getCamera()
{
  //  if(cameras->size()) return (*cameras)[currentCamera];
  if(cameras->size()) return (*(cameras->begin()));
  //  if(cameras->begin()) return (*(cameras->begin()));
  else return 0;
}

//adds a light to the Scene
inline void Scene::addLight(Light * light)
{ //push_bash takes the parameter to add, by reference.
  lights->push_back(light);
}

//adds a surface to the scene
inline void Scene::addSurface(Surface * surface)
{
  surfaces->push_back(surface);
}

//adds a camera to the scene
inline void Scene::addCamera(Camera * camera)
{
  cameras->push_back(camera);
  currentCamera = cameras->size();
}

//adds a material to the scene
inline void Scene::addMaterial(Material * mat)
{
  materials->push_back(mat);
}

//Creates a shader of type shaderType
Shader * Scene::createShader(int shaderType, 
                             Point3Dd matAmbient,
                             Point3Dd matDiffuse,
                             Point3Dd matSpecular,
                             double matSpecExp,
                             double matReflection,
                             double matTransparent
                             )
{
  Shader * rv;
  switch(shaderType)
    {
    case AMBIENT: 
      rv = new LambertShader(matAmbient,
                             Point3Dd(0,0,0),
                             matTransparent,
                             myRenderer
                             );
      break;
    case LAMBERT:
      rv = new LambertShader(matAmbient,
                             matDiffuse,
                             matTransparent,
                             myRenderer
                             );
      break;
#if 0
    case PHONG:
      rv = new PhongShader(matAmbient,
                           matDiffuse,
                           matSpecular,
                           matSpecExp,
                           matTransparent,
                           myRenderer);
      break;
    case PHONGS:
      rv = new PhongSShader(matAmbient,
                            matDiffuse,
                            matSpecular,
                            matSpecExp,
                            matTransparent,
                            myRenderer);
      break;
    case PHONGST:
      rv = new PhongSTShader(matAmbient,
                             matDiffuse,
                             matSpecular,
                             matSpecExp,
                             matTransparent,
                             myRenderer);
      break;
    case PHONGSR:
      rv = new PhongSRShader(matAmbient,
                             matDiffuse,
                             matSpecular,
                             matSpecExp,
                             matTransparent,
                             matReflection,
                             myRenderer);
      break;
    case PHONGSRT:
      rv = new PhongSRTShader(matAmbient,
                              matDiffuse,
                              matSpecular,
                              matSpecExp,
                              matTransparent,
                              matReflection,
                              myRenderer);
      break;
#endif
    default:
      UNIMPLEMENTED("Scene::createShader passed evil shader.\n")
        }
  return rv;
}


// skip the parameters of any non-implemented scene file feature
// assumes that parameter lists are either single strings w/o white space
// or properly parenthetically nested
ifstream& Scene::skipDescription(ifstream& ifFile) {
  char input;

  if(!ifFile.eof()) {
    ifFile >> input;
    // There are two choices for (non-empty) descriptions
    // either it starts with a left paren...
    if(input == '(') {
      int  count = 1;
      while(!ifFile.eof() && count > 0) {
        ifFile >> input;
        switch(input) {
        case '(' :
          count++;
          break;
        case ')' :
          count--;
          break;
        default :
          break;
        }
      }
    }
    // ..or it consists of a single string
    else
      while(!ifFile.eof() && !isspace(ifFile.peek())) ifFile.get();
  }
  return ifFile;
}

void Scene::normalizeChannels()
{
  Point3Dd maxChannels(1,1,1);
  Point3Dd countHigh(1,1,1);
  int max = width*height*3;
  for(int n=0; n<max;)
    {
      if(logicalImage[n++] > maxChannels.x)
        {
          maxChannels.x += logicalImage[n-1];
          countHigh.x++;
        }
      if(logicalImage[n++] > maxChannels.y)
        {
          maxChannels.y += logicalImage[n-1];
          countHigh.y++;
        }
      if(logicalImage[n++] > maxChannels.z)
        {
          maxChannels.z += logicalImage[n-1];
          countHigh.z++;
        }
    }
  maxChannels.x = maxChannels.x / countHigh.x;
  maxChannels.y = maxChannels.y / countHigh.y;
  maxChannels.z = maxChannels.z / countHigh.z;
  if(
     ((1.0/maxChannels.x)!=1.0)
     ||((1.0/maxChannels.y)!=1.0)
     ||((1.0/maxChannels.z)!=1.0)
     )
    {
      std::cout << "Normalizing color channels; scaling over "
                << maxChannels <<std::endl;
      for(int n=0; n<max;)
        {
          logicalImage[n]=logicalImage[n]/maxChannels.x;
          n++;
          logicalImage[n]=logicalImage[n]/maxChannels.y;
          n++;
          logicalImage[n]=logicalImage[n]/maxChannels.z;
          n++;
        }
    }
}
