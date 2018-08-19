# generic makefile for a set of classes for CS371

# (C) Anonymous1 

# Makefile variables

# which compiler
CC = g++
# flags (always use debugging!)
CCFLAGS = -g

LOCALCLASSESDIR ?= ..

# where to find header files
INCLDIRS = -I/usr/cs-local/include/cs371include -I/usr/X11R6/include \
	-I$(LOCALCLASSESDIR)
# where to find libraries
LIBDIRS = -L/usr/cs-local/lib/cs371lib -L/usr/X11R6/lib
# which system libraries to include
LIBS = -llinAlg -lCS371 -lX11 -lm -lc

# these variables are over-ridden locally
CLASS ?= someUnusedValue
PROG ?= someOtherUnusedValue
USESCLASSES ?= someUnusedValue
SUBDIRS ?= subDirsUndefined


$(CLASS).o : $(CLASS).cc $(CLASS).h subdirs
	$(CC) $(CCFLAGS) -c $< $(INCLDIRS)

$(CLASS)Test.o : $(CLASS)Test.cc
	$(CC) $(CCFLAGS) -c $< $(INCLDIRS)

$(CLASS)Test : $(CLASS)Test.o $(CLASS).o
	$(CC) $(CCFLAGS) -o $@ $< $(CLASS).o $(USESCLASSES) \
	$(LIBDIRS) $(LIBS)

$(PROG).o : $(PROG).cc
	$(CC) $(CCFLAGS) -c $< $(INCLDIRS)

$(PROG) : $(PROG).o
	$(CC) $(CCFLAGS) -o $@ $< $(USESCLASSES) \
	$(LIBDIRS) $(LIBS)

subdirs:
	gmake $(SUBDIRS)

clean :
	rm -f *~

# PROGS is defined in each local directory

veryclean :
	make clean
	rm -f $(PROGS)

strip :
	make veryclean
	rm -f *.o
