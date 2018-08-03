include config.mak

CONFIGDIRS=src lib/linAlgLib lib/TomFun

all: $(targets)

beautify:
	cd src; \
	./configure --prefix=$(prefix) $(beautify_config_args); \
	$(MAKE);

libs: bill tom

bill: 
	cd lib/linAlgLib; ./configure --prefix=$(prefix); $(MAKE);

tom:
	cd lib/TomFun; ./configure --prefix=$(prefix); $(MAKE);

clean:
	for i in $(CONFIGDIRS); do \
		pushd $$i; \
		touch config.mak config.h; \
		$(MAKE) clean; \
		popd;\
	done;

mrproper: clean
	for i in $(CONFIGDIRS); do \
		pushd $$i; \
		touch config.mak config.h; \
		$(MAKE) mrproper; \
		popd;\
	done;
	rm config.mak
