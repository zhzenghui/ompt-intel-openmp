VER=deb
HOST:= $(shell hostname -s)

ifeq ($(HOST),manticore)
VER=rel
endif

LIBDIR =../itt/libomp_oss/tmp/lin_32e-rtl_5_nor_dyn.12.$(VER).40.c0.t0-$(HOST)/unstripped
LIB =$(LIBDIR)/libiomp5.so

INCLUDES=-I../itt/libomp_oss/src
HEADERS=${wildcard *.h}

FC = ifort
EXEC = \
	main \
	atomic \
	enumerate-states \
	enumerate-ompd-dll \
	parloop-for-sampling \
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
	single 

# task-1 lateinitf

all: $(EXEC)

clean:
	/bin/rm -f $(EXEC)

$(EXEC) : $(HEADERS) Makefile $(LIB)

%: %.c 
	icc $(INCLUDES) -O0 -g -o $@ -openmp $< -Wl,-rpath=$(LIBDIR) $(LIB)
