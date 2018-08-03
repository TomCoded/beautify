# makefile for liblinAlg.a

# (C) Bill Lenhart 2002
# modified 2003 Tom White

include config.mak

#MAKE = gmake

OBJS = Point3Dd/Point3Dd.o Point4Dd/Point4Dd.o Transform3Dd/Transform3Dd.o \
	Transform4Dd/Transform4Dd.o \
	Point3Df/Point3Df.o Point4Df/Point4Df.o Transform3Df/Transform3Df.o \
	Transform4Df/Transform4Df.o

INCLDIRS = -I.. -I.

#CC = g++

.cc.o:
	$(CC) $(CCFLAGS) -c -o $@ $< $(INCLDIRS)

# build the library
$(prefix)/lib/liblinAlg.a : $(OBJS)
	ar r $@ $?
	chmod 755 $@
	ranlib $@
	#Remember to make sure $(prefix)/lib is in /etc/ld.so.conf
	#And to run ldconfig as root.

all:    config.mak
	cd Point3Dd ; $(MAKE) Point3Dd.o; cd ..
	cd Point4Dd ; $(MAKE) Point4Dd.o; cd ..
	cd Point3Df ; $(MAKE) Point3Df.o; cd ..
	cd Point4Df ; $(MAKE) Point4Df.o; cd ..
	cd Transform3Dd ; $(MAKE) Transform3Dd.o; cd ..
	cd Transform4Dd ; $(MAKE) Transform4Dd.o; cd ..
	cd Transform3Df ; $(MAKE) Transform3Df.o; cd ..
	cd Transform4Df ; $(MAKE) Transform4Df.o; cd ..

copyheaders:
	cp */*.h $(prefix)/include

mrproper: clean
	rm -f config.h config.mak

clean:	
	rm -rf */*.o






