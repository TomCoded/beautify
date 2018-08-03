include ./allclassdirs.mak
include config.mak

CCFILES=$(ALLCLASSDIRS) $(FUNCCS)
OFILES=$(CCFILES:.cc=.o)
CCBACKUPS=$(CCFILES:.cc=.cc~)
HBACKUPS=$(CCFILES:.cc=.h~)
HFILES=$(CCFILES:.cc=.h)
COREFILES=$(CCFILES:.cc=.core)

#OUTDIR=ofiles

CSCCFLAGS = -pthread
PCGLLIBS = -lglut32 -lglu32 -lopengl32

INCLDIRS = $(CSINCLDIRS) 
LIBS = $(magicklibs) -llinAlg -lTomFun -lm $(glutlibs)

TARGET=$(prefix)/bin/beautify

#how to build a .o from a .cc
.cc.o: $(CCFILES)
	$(CC) $(CCFLAGS) -o $@ -c $< $(INCLDIRS)

all: $(TARGET)

$(TARGET): $(OFILES)
	$(CC) $(CCFLAGS) -o $(TARGET) $(OFILES) $(INCLDIRS) $(LIBDIRS) $(LDFLAGS)

profiled: $(OFILES)

kdtest: $(OFILES:Scene/SceneTest.o=PhotonMap/kdTest.o)
	$(CC) $(CCFLAGS) -o kdTest $(OFILES:Scene/SceneTest.o=PhotonMap/kdTest.o) $(INCLDIRS) $(LIBDIRS) $(LIBS)

clean:
	rm -f $(OFILES)
	rm -f $(CCBACKUPS)
	rm -f $(HBACKUPS)	
	rm -f $(COREFILES)

mrproper: clean
	rm config.mak
	rm config.h

#utilities
emacsopen:
	emacs $(CCFILES) $(HFILES) &

print:	
	enscript -2r $(CCFILES) $(HFILES)

wc:
	wc $(CCFILES) $(HFILES)

enscript2r:
	enscript -2r $(HFILES)




