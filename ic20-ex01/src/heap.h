#ifndef heap_h
#define heap_h heap_h

/* Heap structs */
struct heap_element {long id; long value;};
#define HEAPTYPE struct heap_element
#define HEAPGREATER(x,y) (x.value < y.value)
#define PI 3.14159265358979323846f

struct heap { // heap structure
	long size;    // number of elements contained in heap
	long max;     // maximum number of elements (without reallocation)
	HEAPTYPE *s; // integer to storage array
};
#define HEAP struct heap

struct heap * heap_init(long n);

void heap_insert(struct heap *h, HEAPTYPE x);

HEAPTYPE heap_pop(struct heap *h);

void heap_print(struct heap *h);

#endif
