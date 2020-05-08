#include <stdio.h>
#include <stdlib.h>
#include "heap.h"


struct heap* heap_init(long n) 
{
  struct heap* h = (struct heap*)malloc(sizeof(struct heap));
  if (h == NULL) {
    printf("failed to create heap: not enough storage available\n");
    exit(1);
  }
  
  /* heap starts with root node with index 1, i.e. n+1 storage spaces
   * are needed for n elements */
  h->s = (HEAPTYPE*)malloc(sizeof(HEAPTYPE)*(n+1));
  if (h->s == NULL) {
    printf("failed to create heap: not enough storage available\n");
    exit(1);
  }
	
  h->size=0;
  h->max=n;

  //printf("heap with size %ld created, %ld\n",h->max,h->size);
  
  return h;
}

void heap_insert(struct heap *h, HEAPTYPE x) {

  //printf("%ld > %ld?\n",h->size,h->max);
  
  /* check if enough storage available and reallocate if necessary */
  if (h->size>=h->max) {
    //printf("extending heap! \n");
    h->max+=h->max;
    h->s=(HEAPTYPE*)realloc(h->s,(h->max+1)*sizeof(HEAPTYPE));

    if (h->s == NULL) {
      printf("failed to extend heap: not enough storage available\n");
      exit(1);
    }
  }
                          
  /* index for insertion of x, start with last empty child node */
  h->size++;

  //printf("inteserted node into heap (%ld, %f)\n",x.node_ID,x.error);
	
  long i = h->size;
  
  /* insert x into heap */
  while (i>1 && HEAPGREATER(x, h->s[i/2])==1) {
    /* perform swap of parent with the greater child node x */
    h->s[i] = h->s[i/2]; /* store parent node in place of child node */
    i /= 2;              /* set insertion index to former parent node */
  }

  //printf("writing heap node %ld\n",i);
  h->s[i] = x;  
}

/* Return Root-Node and delete it from heap */
HEAPTYPE heap_pop(struct heap *h) {


  
  HEAPTYPE root = h->s[1];

  if (h->size<1){
    printf("error! heap is empty");
    exit(0);
  }
  
  h->s[1] = h->s[h->size];
  (h->size)--;
  HEAPTYPE tmp;

  // printf("removed an element from the heap: %ld\n",h->size);
  
  long i = 1;
  while (i<=h->size/2) {
    long j=2*i;
    if (h->size>=2*i+1 && HEAPGREATER(h->s[i*2+1], h->s[i*2])) j = 2*i+1;
    if (HEAPGREATER(h->s[j], h->s[i])) {
      tmp = h->s[i];
      h->s[i] = h->s[j];
      h->s[j] = tmp;
      i=j;
    } else return root;
  }
  return root;
}

void heap_print(struct heap *h) {
  long i;
  printf("Size: %ld, elements: ", h->size);
  for (i=1; i<=h->size; i++)
    printf(" (%ld,%ld)", h->s[i].id, h->s[i].value);
  printf("\n");
}

