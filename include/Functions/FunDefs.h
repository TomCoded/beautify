#ifndef DEFS_H_
#define DEFS_H_

//#define DEBUG_BUILD
//#define USE_FUNCTIONS

#define SQR(operand) ((operand)*(operand))

#define UNIMPLEMENTED(s) cerr << "The feature you requested, " << s << \
" is unimplemented!\n"; \
exit(1); 

#define BADFORMAT(s) { \
cerr << "Input file syntax error: " << s <<std::endl;\
exit(1); \
}

#ifndef CLONEMETHOD 
#define CLONEMETHOD(s) s * s::clone() { return new s(*this); } 
#endif

#endif



