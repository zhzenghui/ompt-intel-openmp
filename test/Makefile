VER=deb
HOST:= $(shell hostname -s)

ifeq ($(type),rel)
VER=rel
endif

ifeq ($(arch),mic)
ARCH=.knc.lin.off
OPT=-mmic
PREF=lrb
else
PREF=lin
endif

LIBDIR =../itt/libomp_oss/tmp/$(PREF)_32e-rtl_5_nor_dyn.12.$(VER).40.c0.t0$(ARCH)-$(HOST)/unstripped

LIB =$(LIBDIR)/libiomp5.so

INCLUDES=-I../itt/libomp_oss/src
HEADERS=${wildcard *.h}

FC = ifort
EXEC = \
	LULESH_OMP \
	main \
	atomic \
	enumerate-states \
	enumerate-ompd-dll \
	parloop-for-sampling \
	parregion-for-sampling \
	parregion2-for-sampling \
	parregion3-for-sampling-uneven-waiting \
	imbalance \
	lateinit \
	level-1 \
	level-2 \
	level-3 \
	level-4 \
	level-5 \
	master \
	matmult \
	ordered \
	parallelfor \
	single \
	task-1 lateinitf

all: $(EXEC)

clean:
	/bin/rm -f $(EXEC)

$(EXEC) : $(HEADERS) Makefile $(LIB)

%: %.c 
	icc $(INCLUDES) $(OPT) -O0 -g -o $@ -openmp $< -Wl,-rpath=$(LIBDIR) $(LIB)

%: %.cpp 
	icpc $(INCLUDES) $(OPT) -O0 -g -o $@ -openmp $< -Wl,-rpath=$(LIBDIR) $(LIB)

%: %.f 
	ifort $(INCLUDES) $(OPT) -O0 -g -o $@ -openmp $< -Wl,-rpath=$(LIBDIR) $(LIB)
