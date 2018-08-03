# (C) 2003 Tom White

include config.mak
include Funs.mak

OBJS = $(FUNCCS:.cc=.o)

INCLDIRS = -I.. -I.

#CC = g++

.cc.o:
	$(CC) $(CCFLAGS) -c -o $@ $< $(INCLDIRS)

# build the library
$(prefix)/lib/libTomFun.a : $(OBJS) 
	ar r $@ $?
	chmod 755 $@
	ranlib $@
	#Remember to make sure that $(prefix)/lib is listed in /etc/ld.so.conf
	#And to run ldconfig as root.

all:    config.mak $(OBJS) $(prefix)libTomFun.a

copyheaders:
	cp */*.h $(prefix)/include
	cp FunDefs.h $(prefix)/include
	ln -fs $(prefix)/include $(prefix)/include/FunNode

mrproper: clean
	rm -f config.h config.mak

clean:	
	rm -rf $(OBJS)






