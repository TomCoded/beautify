include config.mak

#MAKE = gmake

SRCDIR = src
OBJDIR = obj
BINDIR = bin
TARGET = bin/beautify
INCDIR = inc
#LDFLAGS=$(LDFLAGS) -lTomFun -llinAlg -lglut -lGLU -lGL -L/usr/lib/x86_64-linux-gnu/ #-lMagick++-6.Q16 -lMagick++
GTESTFLAGS=-lgtest_main -lgtest -lpthread -lg

CCFILES1 = $(wildcard $(SRCDIR)/*/*.cc) $(wildcard $(SRCDIR)/*/*/*.cc) $(wildcard $(SRCDIR)/*/*/*/*.cc)
TESTFILES = $(filter-out %Main.cc,$(CCFILES1))
TESTOBJS = $(TESTFILES:.cc=.o)
CCFILES = $(filter-out %Test.cc,$(CCFILES1))
#CCFILES = $(FUNCCS)
#OBJS = $(addprefix $(OBJDIR)/,$(notdir $(CCFILES:.cc=.o)))
#use in place .o files on nested src tree to avoid complex Makefile rules.
OBJS = $(CCFILES:.cc=.o)
INCLDIRS = -I$(SRCDIR) -I$(INCDIR) $(addprefix -I,$(dir $(CCFILES)))
#CC = g++

#build the library
$(TARGET): $(OBJS)
	mkdir -p $(BINDIR)
	$(CC) $(CCFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)
	chmod 755 $@
	#Remember to run make install with permissions on $(prefix)

.cc.o:
	$(CC) $(CCFLAGS) -c -o $@ $^ $(INCLDIRS)

# install the library
install: $(TARGET)
	cp $(TARGET) $(prefix)/bin/

all: config.mak $(OBJS) $(TARGET)

copyheaders:
	cp $(SRCDIR)/*/*.h $(prefix)/include

mrproper: clean
	rm -f config.h config.mak

clean:	
	rm -f bin/beautify
	rm -rf $(OBJS) $(TESTOBJS)

test: $(TESTOBJS) 
	$(CC) $(CCFLAGS) $(TESTOBJS) -o bin/test $(LDFLAGS) $(GTESTFLAGS) $(GTESTLIBDIRS)
	bin/test





