GPP=gcc
CCFLAGS=-msse -Wall
CCFLAGS+=-g
CCFLAGS+=-O3
LDFLAGS=-lm 

OBJECTS=src/bfio.o \
	src/image_io.o \
	src/alloc.o 

all: compress

compress: $(OBJECTS) src/ic20_jpeg_light.c Makefile
	$(GPP) $(CCFLAGS) $(OBJECTS) src/ic20_jpeg_light.c -o ic20_jpeg_light $(LDFLAGS)

%.o : %.c
	$(GPP) $(CCFLAGS) -o $@ -c $<

clean:
	rm -f compress
	rm -f src/*.o src/*~
	rm -f *.o *~
