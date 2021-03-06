#ifndef DEFS_H_
#define DEFS_H_

//#define DEBUG_BUILD
//#define USE_FUNCTIONS

#define MAX_FILENAME_LEN 256

#define TAG_HANDSHAKE 6000
#define TAG_PROGRAM_DONE 7000
#define MASTER_PROCESS 0

//how much larger to make the map than number of emitted photons
#define MAPMULT 50

//Model for computing specular reflection
#define HALFWAY 1
//#define PHONG 2

//for Renderer::map()
#define DEFAULT_PHOTONS 1500000

#define ORIGIN Point3Dd(0,0,0)
#define BACKGROUND 0.0,0.0,0.0
//all diffuse shadings get multiplied by this, if it is defined
#define DIFFUSE 1.5
//maximum recursion depth for reflection
#define MAXREFLECTIONDEPTH 2

//distance to bump hitpoints out from a surface
#define BUMPDISTANCE 0.001

#define SQR(operand) ((operand)*(operand))

#define UNIMPLEMENTED(s) std::cerr << "The feature you requested, " << s << \
" is unimplemented!\n"; \
exit(1); 

#define ASSERT(condition,errMsg) {					\
    if(!condition)							\
    { std::cerr << "An assertion failed: " << errMsg << std::endl;	\
      exit(1); }							\
  }

#define ASSERT_WARN(condition,errMsg) {					\
    if(!condition)							\
    { std::cerr << "An assertion failed: " << errMsg << std::endl;	\
      }							\
  }

#define BADFORMAT(s) {						\
std::cerr << "Input file syntax error: " << s <<std::endl;\
exit(1); \
}

#define FORMATTEST(value,shouldBe,errString) { \
    if (value != shouldBe) { BADFORMAT(errString + ":" + value + " found instead of " + shouldBe) } \
    }


#ifndef CLONEMETHOD 
#define CLONEMETHOD(s) s * s::clone() { return new s(*this); } 
#endif

//constants for Russian Roulette.
//returned by the Material::nRoulette function
#define ABSORPTION 0
#define DIFFUSE_REFLECTION 8
#define SPECULAR_REFLECTION 16
#define SCATTER 32
#define STEP 64



#endif



