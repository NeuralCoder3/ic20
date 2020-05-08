GPP=gcc
CCFLAGS=-msse -Wall
CCFLAGS+=-g
#CCFLAGS+=-O3
LDFLAGS=-lm 

OBJECTS=src/bfio.o \
	src/image_io.o \
	src/alloc.o \
	src/heap.o

all: huffman

huffman: $(OBJECTS) Makefile
	$(GPP) $(CCFLAGS) $(OBJECTS) src/ic20_huffman.c -o huffman $(LDFLAGS)

%.o : %.c
	$(GPP) $(CCFLAGS) -o $@ -c $<

clean:
	rm -f huffman
	rm -f src/*.o src/*~
	rm -f *.o *~
