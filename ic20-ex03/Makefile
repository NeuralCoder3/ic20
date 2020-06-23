CC=gcc
CCFLAGS=-msse -Wall
#CCFLAGS+= -g
CCFLAGS+=-O3
LDFLAGS=-lm 

all: quantisation

quantisation: ic20_quantisation.c Makefile
	$(CC) $(CCFLAGS) $(OBJECTS) ic20_quantisation.c -o ic20_quantisation $(LDFLAGS) 

%.o : %.c
	$(CC) $(CCFLAGS) -o $@ -c $<
clean:
	rm -f ic20_quantisation
	rm -f *.o *~
