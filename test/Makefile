VER=deb
VER=rel
HOST=mic
HOST=manticore

LIBDIR =../itt/libomp_oss/tmp/lin_32e-rtl_5_nor_dyn.12.$(VER).40.c0.t0-$(HOST)/unstripped
LIB =$(LIBDIR)/libiomp5.so

INCLUDES=-I../itt/libomp_oss/src
HEADERS=${wildcard *.h}

FC = ifort
EXEC = main level-1 level-2 level-3 level-4 lateinit matmult pragmas # task-1 lateinitf

all: $(EXEC)

clean:
	/bin/rm -f $(EXEC)

%: %.c $(HEADERS) Makefile $(LIB)
	icc $(INCLUDES) -g -o $@ -openmp $< -Wl,-rpath=$(LIBDIR) $(LIB)
