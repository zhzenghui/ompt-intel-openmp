VER=rel
VER=deb

LIBDIR =../itt/libomp_oss/tmp/lin_32e-rtl_5_nor_dyn.12.$(VER).40.c0.t0-mic/unstripped
LIB =$(LIBDIR)/libiomp5.so

INCLUDES=-I../itt/libomp_oss/src

FC = ifort
EXEC = main level-1 level-2 level-3 level-4 task-1 lateinit # lateinitf

all: $(EXEC)

clean:
	/bin/rm -f $(EXEC)

%: %.c
	icc $(INCLUDES) -g -o $@ -openmp $< -Wl,-rpath=$(LIBDIR) $(LIB)
