CC=gcc
CCFLAGS=-msse -Wall
#CCFLAGS+= -g
CCFLAGS+=-O3
LDFLAGS=-lm 

all: mask inpainting

mask: mask_generator.c Makefile
	$(CC) $(CCFLAGS) $(OBJECTS) mask_generator.c -o mask_generator $(LDFLAGS)

inpainting: homogeneous_inpainting.c Makefile
	$(CC) $(CCFLAGS) $(OBJECTS) homogeneous_inpainting.c -o inpainting $(LDFLAGS) 

%.o : %.c
	$(CC) $(CCFLAGS) -o $@ -c $<

clean:
	rm -f mask_generator
	rm -f inpainting
	rm -f *.o *~
