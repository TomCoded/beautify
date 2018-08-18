// This may look like C code, but it is really -*- C++ -*-

// Scene.cc

// (C) 2002 Tom White

#include <allIncludes.h>
#include <iostream>
#include <string>
#include <sys/time.h>
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
void saveMap(PhotonMap * pmap, std::string fileName);

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
PhotonMap * g_volMap;
#define rayTrace 0
#define photonMap 1

double lastTime;
double curTime;
bool hasLastTime;

bool g_parallel;
bool g_suppressGraphics;

//does anything move around between frames?
bool g_dynamicMap=false;

char outbuffer[120];

std::vector<long> frameTimes;

////////////////////
// class Scene //
////////////////////

// A scene 
//


void frameCount();

//glut display callback function
void display()
{
  g_Scene->glutDisplayCallback();
}

void Scene::startMainLoop(int rank) {
  this->rank=rank;
  
  if(!rank) { 
    if(!g_suppressGraphics) {
      glutMainLoop();
    } else {
      mainLoop();
    }
  } else {
    mainLoop();
  }
}

void Scene::glutDisplayCallback() {
  mainLoop();
}

void Scene::mainLoop() {
  bool done=false;
  //quit program at end
  bool quit=false;
  MPI_Request doneRequest;

  //listen for shutdown flags
  if(rank) { 
    MPI_Irecv(&done,1,MPI_C_BOOL,
	       MASTER_PROCESS,
	      TAG_PROGRAM_DONE,
	       MPI_COMM_WORLD
	       ,&doneRequest
	       );
  }
  
  while(!done) {
    if(!rank) frameCount();
    currentTime+=dtdf;
    drawSingleFrame(currentTime);
    
    if(!rank) {

      if(writesThisManyFrames) {
	renderDrawnSceneToFile();
      }

      if(!g_suppressGraphics) {
	renderDrawnSceneToWindow();
      }
      
    }
    
    frame++;
    if(writesThisManyFrames &&
       frame>=startsOnFrame+writesThisManyFrames) {
      done=true;
    }
    //In Master process, this whole function is within
    //and driven by the glutMainLoop.
    if(!rank) {
      if(!g_suppressGraphics) {
	done=true;
      }
    }
    
#ifdef PARALLEL
    if(g_parallel) {
      if(!rank) {
	if (g_suppressGraphics ||
	    (writesThisManyFrames && frame>=writesThisManyFrames))
	  {
	    closeChildren();
	    quit = true;
	  }
      }
    }
#endif
    
  }

#ifdef PARALLEL
  if(g_parallel) {
    if(rank) quit = true;
    if(quit) {
      MPI_Barrier(MPI_COMM_WORLD);
      MPI_Finalize();
      std::cout << "Finalized process " << rank << std::endl;
      exit(0);
    }
  }
#endif
  
}

void frameCount() {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  long actualTime=
    tp.tv_sec * 1000 + tp.tv_usec / 1000;
  frameTimes.push_back(actualTime);
  int frameCount = frameTimes.size();
  int frameTimesToAvg = 5>frameTimes.size() ? frameTimes.size() : 5;
  double frameRateRecentAverage =
    (frameTimes[frameTimes.size()-1]
     -frameTimes[frameTimes.size()-frameTimesToAvg]
     ) / frameTimesToAvg;
  std::cout << 1/(frameRateRecentAverage/1000.0) << " fps." << std::endl;
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
          std::string fileName;
          std::cin >> fileName;
          g_Scene->renderDrawnSceneToFile();
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
      if(g_parallel) {
	g_Scene->closeChildren();
      }
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
  std::vector<Surface *>::iterator itSurf;
  std::vector<Camera *>::iterator itCam;

  curTime=t;
  hasLastTime=true;

  for(itSurf=surfaces->begin();
      itSurf!=surfaces->end();
      itSurf++)
    (*itSurf)->setTime(t);
  for(itCam=cameras->begin();
      itCam!=cameras->end();
      itCam++)
    (*itCam)->setTime(t);

  lastTime=curTime;
}

// default constructor
Scene::Scene():
  currentCamera(0),
  width(300),
  height(300),
  hasImage(0),
  dtdf(0),
  logicalImage(0),
  paintingFromLogical(false),
  numNeighbors(0),
  nPhotons(6000),
  minDist(0.5),
  maxDist(2.0),
  frame(0),
  startsOnFrame(0),
  writesThisManyFrames(0),
  mapCreationTime(0.0),
  treeCreationTime(0.0),
  frames_are_being_rendered_to_files(false)
{
  lights = new std::vector<Light *>();
  surfaces = new std::vector<Surface *>();
  cameras = new std::vector<Camera *>();
  materials = new std::vector<Material *>();
  photonMaps = new std::vector<PhotonMap *>();
  g_specModel=HALFWAY;
  myRenderer = new Renderer(this);
  g_Scene=this;
  Magick::InitializeMagick("/usr/lib");
  if(!g_suppressGraphics) {
    int rank=0;
#ifdef PARALLEL
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
#endif
    if(!(rank)) {
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
}

// destructor
Scene::~Scene() 
{
  //  if(g_map)
  //    delete g_map;
  if(hasImage)
    delete[] logicalImage;

  std::vector<Light *>::iterator itDestroyer = lights->begin();
  while( itDestroyer != lights->end() )
    { //destroy all light sources in the std::vector
      if(*itDestroyer) delete (*itDestroyer);
      itDestroyer++;
    }
  delete lights;

  std::vector<Surface *>::iterator itSDestroyer = surfaces->begin();
  while( itSDestroyer != surfaces->end() )
    { //destroy all surfaces in the std::vector
      delete (*itSDestroyer);
      itSDestroyer++;
    }
  delete surfaces;

  std::vector<Material *>::iterator itMatDestroyer = materials->begin();
  while( itMatDestroyer != materials->end() )
    { //destroy all Materials in the std::vector
      delete (*itMatDestroyer);
      itMatDestroyer++;
    }
  delete materials;

  std::vector<Camera *>::iterator itCDestroyer = cameras->begin();
  while(itCDestroyer != cameras->end() )
    {
      delete (*itCDestroyer);
      itCDestroyer++;
    }
  if(myRenderer) delete myRenderer;
}

// Scene features
void Scene::ReadFile(std::string fileName) {
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
  std::ifstream inFile(fileName.c_str());

  // check for success of stream constructor
  if(!inFile) { // could not open file
    std::cerr << "Could not open file: " << fileName << " exiting...";
    exit(1);
  }

  // read in contents
  std::string keyword;

  while(!inFile.eof()) {
    inFile >> keyword;

    if(keyword == "include") {
      std::string ifile;
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
      std::string formatErr("Bad format for rotation");
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
        delete[] logicalImage;
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
  if(x<0) x=0;
  if(x>=width) x=width-1;
  if(y<0) y=0;
  if(y>=height) y=height-1;
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
//TODO: Remove if not used any longer.
//  relied on old putPixel method
void Scene::repaint()
{
  ASSERT(false,"repaint() no longer effective. Call RenderDrawnSceneToWindow()")
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

void Scene::setWindowSize(int x, int y)
{
  width=x;
  height=y;
  if(!g_suppressGraphics)
    glutReshapeWindow(x,y);
  if(hasImage&&logicalImage)
    delete[] logicalImage;
  logicalImage = new double[width*height*3];
  hasImage=true;
}

int Scene::getWindowWidth()
{ return width; }

int Scene::getWindowHeight()
{ return height; }

double * Scene::toLogicalImage()
{
  myRenderer->ambient=ambient;

  if(photonMap&&(!g_map)) {
    g_map = g_Scene->myRenderer->map(nPhotons);
    photonMaps->push_back(g_map);
  }
  if(photonMap&&(!g_volMap)) {
    g_volMap = g_Scene->myRenderer->getVolMap();
    photonMaps->push_back(g_volMap);
  }

#ifdef PARALLEL
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  if(g_parallel) {
    MPI_Barrier(MPI_COMM_WORLD);
    toLogicalImageInParallel();
  } else 
#endif
  myRenderer->showMap(g_map);
  
  return logicalImage;
}

#ifdef PARALLEL
void Scene::toLogicalImageInParallel() {
  int nodes, rank;
  double start, stop; //for timings
  MPI_Comm_size(MPI_COMM_WORLD,&nodes);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  int totalSize = getWindowHeight()*getWindowWidth();
  int task = rank;

  //allocate space for a row of pixels.
  localLogicalSize = getWindowWidth();

  //each process starts off with row equal to its rank
  localLogicalStart = rank * localLogicalSize;
  localLogical = new double[3*localLogicalSize];
  doingLocalPart = true;

  //distribute kdTree
  if(!map_too_small) {
    start = MPI_Wtime();

    int mapSize=(*(photonMaps->begin()))->getSize();
    
    for(auto &pMap: *photonMaps) { pMap->distributeTree(); }
#ifdef SAVE_VOLMAP
    if(!rank) {
      saveMap(myRenderer->getVolMap(),"volmap.map");
    }
#endif

    stop = MPI_Wtime();
    //    printf("%s",outbuffer);
    sprintf(outbuffer,"%5d  %4d  %5d %9d  %9f ",
            g_nFrame, rank, nodes, mapSize,
            stop - start
            );

    //now actually render
    start = MPI_Wtime();
    MPI_Status status;
    bool done=false;

    if(rank) {
      while(!done) {
        //we are a child process.  Have default task.  Run task.

        //throw rays from the eye toward the scene, calling putPixel()
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
            if(!(progressMeter % (height/80)))
              std::cout << "|" << std::flush;
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
            myRenderer->showMap(g_map,
                                taskPlace++,
                                1
                                );
            if(taskPlace%width == 0) {
              //now copy stuff over to logicalImage from local
              //	      int done = task*3*width+localLogicalSize*3;
              progressMeter++;
              if(!(progressMeter % (height/80)))
                std::cout << "|" << std::flush;

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
    //sprintf(outbuffer + strlen(outbuffer), "%6f\n",stop-start);
    sprintf(outbuffer + strlen(outbuffer), "%6f %9f %6f\n",stop-start,mapCreationTime, treeCreationTime);
  } 
  if(!rank) {
    std::cout <<std::endl;
    //std::cerr << "Volumetric Photon Map is size " << myRenderer->getVolMap()->getSize() <<std::endl;
  }

  if(!frame && !rank) {
    //std::cout << "frame rank nodes mapsize    disttrees render " << std::endl;
    std::cout <<std::endl;
    std::cout << "frame  proc  procs pMap Size  pMap dist render   pMap create kdTree\n";
    std::cout << "-----  ----  ----- ---------  --------- ------- ----------- ------\n";
  }
  printf("%s",outbuffer);
  //now gather information again.  Everything has same amount of data,
  //use gather.
  start = MPI_Wtime();

  //Now we have all the data we need.  Delete local logical buffer.
  delete [] localLogical;
  doingLocalPart = false;
  MPI_Barrier(MPI_COMM_WORLD);
  stop = MPI_Wtime();
  
  //if(!rank)
  //  printf("%d %d %d Gather Required: %f s\n",g_nFrame,rank,nodes,stop-start);
  //      std::cout << "Took " << stop - start << " seconds to gather pixels.\n";
  hasImage = true;

}
#endif

//returns a pointer to a std::vector of pointers to all lights in the scene
std::vector<Light *> * Scene::getLights()
{
  return lights;
}

//returns a pointer to a std::vector of pointers to all surfaces in the scene
std::vector<Surface *> * Scene::getSurfaces()
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
// assumes that parameter std::lists are either single std::strings w/o white space
// or properly parenthetically nested
std::ifstream& Scene::skipDescription(std::ifstream& ifFile) {
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
    // ..or it consists of a single std::string
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

#define NEIGHBORHOODS_PER_SURFACE 10
void Scene::setNumNeighbors(int numNeighbors) {
  this->numNeighbors = numNeighbors;
  setNumNeighbors();
}

void Scene::setNumNeighbors() {
  if(numNeighbors) {
    for(auto &pMap: *photonMaps) {
      pMap->setNumNeighbors(numNeighbors);
    }
  } else {
    for(auto &pMap: *photonMaps) {
      pMap->setNumNeighbors(surfaces->size(),
			    NEIGHBORHOODS_PER_SURFACE
			    );
    }
  }
}

void Scene::willWriteFilesWithBasename(const char *filename) {
  frames_are_being_rendered_to_files=true;
  strncpy(szFile,filename,MAX_FILENAME_LEN);
  nBaseLen = strlen(filename);
  ASSERT((nBaseLen<201),"Base file name too long.");
  
  int rank=0;
#ifdef PARALLEL
  int nodes;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&nodes);
  if(!rank) {
    std::cout <<std::endl;
    std::cout << "frame  proc  procs pMap Size  pMap dist render   pMap create kdTree\n";
    std::cout << "-----  ----  ----- ---------  --------- ------- ----------- ------\n";
  }
#endif
}

void Scene::willWriteThisManyFrames(int willWriteThisManyFrames) {
  this->writesThisManyFrames=willWriteThisManyFrames;
}

void Scene::willStartOnFrame(int willStartOnFrame) {
  this->startsOnFrame=willStartOnFrame;
  this->frame=willStartOnFrame;
  g_nFrame=frame;
}

void Scene::willHaveThisManyPhotonsThrownAtIt(int nPhotons) {
  this->nPhotons = nPhotons;
}

void Scene::willNotUseMoreThanThisManyPhotonsPerPixel(int neighbors) {
  setNumNeighbors(neighbors);
}

void Scene::willNeverDiscardPhotonsThisClose(int minDist) {
  this->minDist = minDist;
}

void Scene::willAlwaysDiscardPhotonsThisFar(int maxDist) {
  this->maxDist = maxDist;
}

void Scene::tuneMapParams() {
  setNumNeighbors();
  if(minDist) {
    for(auto &pMap: *photonMaps) {
      pMap->setMinSearch(minDist);
    }
  }
  if(maxDist) {
    for(auto &pMap: *photonMaps) {
      pMap->setMaxDist(maxDist);
    }
  }
}

void Scene::drawSingleFrame(double time) {
  curTime=time;
  if(photonMap) {
    if(hasLastTime && (lastTime==curTime)) {
      if(hasImage) {
	//this frame already exists in memory
	return;
      }
    } else {
      //update the time of the scene
      setTime(curTime);
      //throw photons into the world
      PhotonMap * pm = myRenderer->map(nPhotons);
      while(photonMaps->size()) { photonMaps->pop_back(); }
      photonMaps->push_back(pm);

#ifdef PARALLEL
      if(g_parallel) {
	double start=MPI_Wtime();
	//gather photons from other nodes
	for(auto &pMap: *photonMaps) {
	  pMap->gatherPhotons(nPhotons);
	}
	double stop=MPI_Wtime();
	mapCreationTime=stop-start;
      }
#endif
      
      //tune the photon map
      tuneMapParams();
      setNumNeighbors();

      //build (or re-build) the kd-tree
      double start=MPI_Wtime();
      for(auto &pMap: *photonMaps) {
	pMap->buildTree();
      }
      double stop=MPI_Wtime();
      treeCreationTime=stop-start;
    }
    //draw scene to our logical image.
    toLogicalImage();
  } else if(rayTrace) {
    //The very early skeleton of the Photon Mapper came from
    //an old undergrad ray tracing assignment. It would be
    //easy to add a ray tracer back in and I may do so in the
    //future.
    ASSERT(false,"Ray Tracing Not Enabled.");
  } else {
    ASSERT(false,"NO ILLUMINATION ENGINE SPECIFIED");
  }
}

//slower per frame than using GLUT pipeline directly in putPixel,
//but much better encapsulation.
//note: only call on process you want to use GLUT.
void Scene::renderDrawnSceneToWindow() {
  int totalPixelDoubles = width*height*3;
  double r, g, b, dx, dy, x, y;
  glutReshapeWindow(width,height);

  for(int x=0; x<width; x++) {
    for(int y=0; y<height; y++) {
      int nPlace= (y*width+x)*3;
      glPointSize(1.5);
      r=logicalImage[nPlace];
      g=logicalImage[nPlace+1];
      b=logicalImage[nPlace+2];
      dx = (x/(double)width);
      dy = (y/(double)height);
      glColor3d(r,g,b);
      glBegin(GL_POINTS);
      glVertex3d(dx,dy,0.0);
      glEnd();
    }
  }
  
  glFlush();
  glutPostRedisplay();
}

void Scene::setFilename() {
  ASSERT((frame<10000000),"Cannot Process a frame count that high.");
  sprintf(szTail,"%d.jpg\x00",frame);
  std::cout<<"np.";
  strncpy(&szFile[nBaseLen],szTail,strlen(szTail)+1);
  std::cout<<"wtf.";
}

//was Scene::writeImage()
void Scene::renderDrawnSceneToFile() {
  setFilename();
  const char * filename=szFile;
  std::cout << "Writing to " << filename << std::endl;
  
  char dims[32];
  sprintf(dims,"%dx%d\x00",width,height);
  //  Image img(dims,"white");
  Magick::Image img (Magick::Geometry(width,height),"white" );
  int nPos=0;
  if(hasImage&&logicalImage) {
    for(int y=0; y<height; y++) {
      for(int x=0; x<width; x++) {
	  img.pixelColor(x,
			 y,
			 Magick::ColorRGB(logicalImage[nPos],
					  logicalImage[nPos+1],
					  logicalImage[nPos+2]
					  )
			 );
	  nPos+=3;
      }
    }
  }

  //This keeps ffmpeg from freezing on blank frames
  //in videos we later create
  img.pixelColor(0,0,Magick::ColorRGB(1.0,1.0,5.0));
  img.pixelColor(1,1,Magick::ColorRGB(1.0,1.0,5.0));
  img.pixelColor(2,1,Magick::ColorRGB(1.0,1.0,5.0));
  img.pixelColor(1,2,Magick::ColorRGB(1.0,1.0,5.0));
  img.pixelColor(2,2,Magick::ColorRGB(1.0,1.0,5.0));

  img.flip();
  img.write(filename);
}

void Scene::closeChildren() {
  bool done=true; int nodes=0;
  if(!rank) {
    //MPI_Abort(MPI_COMM_WORLD,0);
    //exit(0);

    int nodes=MPI_Comm_size(MPI_COMM_WORLD, &nodes);
    for(int i=1; i<nodes; i++) {
      MPI_Send(&done,1,MPI_C_BOOL,
	       i,
	       TAG_PROGRAM_DONE,
	       MPI_COMM_WORLD
	       );
    }
  }
}

