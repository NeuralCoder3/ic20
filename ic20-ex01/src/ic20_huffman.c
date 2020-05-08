/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                  PROGRAMMING EXERCISE FOR THE LECTURE                    */
/*                            IMAGE COMPRESSION                             */
/*                             HUFFMAN CODING                               */
/*                  (Copyright by Pascal Peter, 3/2018)                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/* global includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <getopt.h>
#include <omp.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

/* local includes */
#include "alloc.h"              /* memory allocation */
#include "image_io.h"           /* reading and writing pgm and ppm images */
#include "bfio.h"               /* writing and reading of bitfiles */
#include "heap.h"               /* heap implementation */

/* defines */
/* version */
#define VERSION 1.23-03-17
/* supported input formats */
#define FORMAT_PGM 0
#define FORMAT_PPM 1
/* auxiliary functions */
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
/* maximum number of channels */
#define MAXCHANNELS 3
/* define maximum grey value */
#define MAXGREYVALUE 255
/* console formatting */
#define ONE_UP "\033[5D\033[1A"
#define CLRLINE "\033[K"

/* definition of compressed image datatype and struct */
typedef struct ImageData ImageData;
struct ImageData {
  long*** orig_rgb;    /* original image (RGB) */
  long size_orig;      /* size of raw input image */
  long nx, ny, nc;     /* image dimensions and channels */
  long encoder;        /* encoder to apply */
  long** orig_hist;    /* channel histograms of original image */
  double** orig_prob;  /* channel probabilities of original image */ 
};

/*long log2(long x) {
  int logx = 0;
  while (x >>= 1) ++logx;
  return logx;
  }*/

/*--------------------------------------------------------------------------*/
void init_image (ImageData* img) {
  /* set image variables to default values */
  img->nx=img->ny=img->nc=0;
  img->encoder=0;
  img->size_orig=0;
  img->orig_rgb=0;
  img->orig_hist=0;
  img->orig_prob=0;
}

/*--------------------------------------------------------------------------*/
void alloc_image (ImageData* img,long nx,long ny) {
  /* allocate all image data arrays */
  if (img->orig_rgb == 0)
    alloc_long_cubix(&img->orig_rgb,MAXCHANNELS,nx+2,ny+2);
  if (img->orig_hist == 0)
    alloc_long_matrix(&img->orig_hist,MAXCHANNELS,MAXGREYVALUE);
  if (img->orig_prob == 0)
    alloc_matrix(&img->orig_prob,MAXCHANNELS,MAXGREYVALUE);
}

/*--------------------------------------------------------------------------*/
void destroy_image (ImageData* img) {
  /* disalloc all image data arrays */
  long nx, ny;
  nx = img->nx;
  ny = img->ny;
  /* disalloc images */
  if (img->orig_rgb != 0)
    disalloc_long_cubix(img->orig_rgb,MAXCHANNELS,nx+2,ny+2);
  if (img->orig_hist != 0)
    disalloc_long_matrix(img->orig_hist,MAXCHANNELS,MAXGREYVALUE);
  if (img->orig_prob != 0)
    disalloc_matrix(img->orig_prob,MAXCHANNELS,MAXGREYVALUE);
}

/*--------------------------------------------------------------------------*/
void write_comment_string(ImageData* img, char* additional, char* comments)
/* writes all important information of an R-EED image into a comment string
 * parameter additional allows to add more custom text, set to 0 if not needed.
 * make sure that the comments array is preallocated with sufficient space. */
{
  /* write parameter values in comment string */
  comments[0] = '\0';
  comment_line (comments, (char*)"# IMAGE COMPRESSION - PROGRAMMING EXERCISE\n");
}

/*--------------------------------------------------------------------------*/
/* for user interaction */
/* Returns true if it is possible to read from cin */
int canread()
{
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  fd_set read_set;
  FD_ZERO(&read_set);
  FD_SET(0,&read_set);
  select(1,&read_set,NULL,NULL, &tv);
  return (FD_ISSET(0,&read_set));
}

/*--------------------------------------------------------------------------*/
void copy_cubix(double ***source, double ***target,
		long sc, long nc, long nx, long ny) {
  /* copy cubix from channel sc to channel nc */
  long i,j,m;
  for (m=0;m<nc;m++) {
    for (i=1;i<=nx;i++) {
      for (j=1;j<=ny;j++) {
        target[m][i][j]=source[m][i][j];
      }
    }
  }
}

/*--------------------------------------------------------------------------*/
void copy_cubix_long(long ***source, long ***target,
                    long sc, long nc, long nx, long ny) {
  /* copy cubix (only nc channels, starting with channel sc) */
  long i,j,m;
  for (m=sc;m<nc;m++) {
    for (i=1;i<=nx;i++) {
      for (j=1;j<=ny;j++) {
        target[m][i][j]=source[m][i][j];
      }
    }
  }
}

/*--------------------------------------------------------------------------*/
void copy_matrix_long(long **source, long **target,long nx, long ny) {
  /* copy input matrix to target matrix */
  long i,j;
  for (i=0;i<nx;i++) {
    for (j=0;j<ny;j++) {
      target[i][j]=source[i][j];
    }
  }
}

/*--------------------------------------------------------------------------*/
void copy_vector_long(long *source, long *target, long nx) {
  /* copy input vector to target vector */
  long i;
  for (i=0;i<nx;i++) {
    target[i]=source[i];
  }
}

/*--------------------------------------------------------------------------*/
long get_size_of_file(char* filename) {
  /* compute and return size of file with arbitrary content */
  FILE   *infile;             /* input file */
  long i;
  /* open file */
  infile = fopen(filename,"r");
  if (NULL == infile)
    {
      printf("Could not open file '%s' for reading, aborting (10)\n",filename);
      exit(-1);
    }
  /* Find length of file */
  fseek(infile,0,SEEK_END);
  i = ftell(infile);
  fclose(infile);
  return i;
}

/*--------------------------------------------------------------------------*/
double get_compression_ratio(char* original_file,
                             char* compressed_file) {
  /* computes compression ratio between two files on the hard disk (X:1) */
  long original_size;
  long compressed_size;
  
  original_size = get_size_of_file(original_file);
  compressed_size = get_size_of_file(compressed_file);
  
  return (double)original_size/(double)compressed_size;
}

/*--------------------------------------------------------------------------*/
void convert_image_to_vector (long*** image_matrix,
                              long bx, long by, long nx, long ny, long nc,
                              long* image_vector) {
  long i,j,c;

  /* parse image channel by channel, row by row */
  for (c = 0; c < nc; c++) 
    for (j = by; j < ny+by;j++)
      for (i = bx; i < nx+bx;i++) {
        image_vector[c*nx*ny+(j-by)*nx+(i-bx)]=image_matrix[c][i][j];
      }
}

/*--------------------------------------------------------------------------*/
void convert_vector_to_image (long* image_vector,
                              long bx, long by, long nx, long ny, long nc,
                              long*** image_matrix) {
  long i,j,c;

  /* parse image channel by channel, row by row */
  for (c = 0; c < nc; c++) 
    for (j = by; j < ny+by;j++)
      for (i = bx; i < nx+bx;i++) {
        image_matrix[c][i][j]=image_vector[c*nx*ny+(j-by)*nx+(i-bx)];
        /*  printf("%ld %ld %ld (%ld): %ld\n",
           c,i,j,c*nx*ny+(j-by)*nx+(i-bx),image_matrix[c][i][j]); */ 
      }
}

/*--------------------------------------------------------------------------*/
void print_usage_message() {
  printf("./compress -i input_file -o output_prefix [optional parameters]\n");
  printf("list of mandatory paramters:\n");
  printf("-i input_file   (string): uncompressed image, e.g. \"image.pgm\"\n");
  printf("-o out_prefix   (string): prefix for output files, e.g. \"my_image\"\n");
  printf("list of optional paramters:\n");
  printf("-D debug file   (string): filename for debug information");  
}

/*--------------------------------------------------------------------------*/

struct tree_node {
  struct tree_node* parent;
  struct tree_node* left;
  struct tree_node* right;
  long id;
};
#define TREENODE struct tree_node

/*--------------------------------------------------------------------------*/

struct tree {
  struct tree_node* nodes;
  struct tree_node root;
  long max_nodes;
  long node_number;
};
#define TREE struct tree

/*--------------------------------------------------------------------------*/
TREE* tree_init(long n) {
  long i;
  TREE* t = (TREE*)malloc(sizeof(TREE));
  if (t == NULL) {
    printf("failed to create tree: not enough memory available\n");
    exit(1);
  }
  t->nodes = (TREENODE*)malloc(sizeof(TREENODE)*n);
  if (t->nodes == NULL) {
    printf("failed to create tree: not enough memory available\n");
    exit(1);
  }
  t->node_number = 0;
  t->max_nodes=n;
  for (i=0;i<t->max_nodes;i++) {
    t->nodes[i].parent = 0;
  }
  return t;
}

/*--------------------------------------------------------------------------*/
TREE* build_huffman_tree(
           long s,            /* size of source alphabet */
           long* histogram,   /* occurrence frequency for each symbol */
           FILE* debug_file) {/* 0 - no output, else - debug
                                                output to file */

  /* construct huffman tree for alphabet of size s, probabilities are given
     in terms of occurrence counters for symbols 0,...,s-1 in the array
     histogram */
  
  HEAP* h;               /* heap for ordering tree nodes */
  HEAPTYPE x,y,z;        /* heap elements */
  long n;                /* heap size */
  long i;                /* loop variable */
  long id_counter;       /* node ids */
  TREENODE node;         /* tree node */
  TREE* htree;           /* huffman tree */

  /* HINT: using the heap.
     HEAPTYPE has two data fields (long): x.id and x.value 
     insert heap element: heap_insert(h,x);
     retrieve heap element with smallest value: x=heap_pop(h); */

  /* compute maximum number of nodes, initialise tree and heep */
  n = (long)pow(2,ceil(log((float)s)/log(2.0))+1);
  h = heap_init(n);
  htree = tree_init(n);
  
  if (debug_file != 0) {
    fprintf(debug_file,"\nbuilding huffman tree (maximum %ld nodes)\n",n);
  }

  /* insert leaf nodes into heap and tree */
  for (i=0;i<s;i++) {
    x.id = i;
    x.value = histogram[i];
    heap_insert(h,x);
    node.id = i;
    node.left = 0;
    node.right = 0;
    htree->nodes[i] = node;
    htree->node_number++;
    if (debug_file != 0) {
      fprintf(debug_file,"added leaf node %ld: histogram count %ld\n",
              x.id,x.value);
    }
  }

  id_counter = s;

  /* create tree */
  while (h->size > 0) {

    /* TODO: assign two heap elements with smallest symbolcounts to x and y */

    if (htree->node_number >= htree->max_nodes) {
      printf("Error in build_huffman_tree: tree structure ran out of memory\n");
      exit(-1);
    }
    /* TODO: merge two nodes with smallest probability */
    z.id = id_counter;
    id_counter++;
    z.value = /* TODO */
    node.id = z.id;
    node.left = /* TODO */
    node.right = /* TODO */
    node.parent = 0;
    htree->nodes[z.id]=node;
    htree->nodes[x.id].parent = &(htree->nodes[z.id]);
    htree->nodes[y.id].parent = &(htree->nodes[z.id]);

    if (debug_file != 0) {
      fprintf(debug_file,"merged nodes %ld and %ld, new node %ld, count %ld\n",
              x.id,y.id,z.id,z.value);
    }
    
    /* insert new node into heap if necessary */
    if (h->size > 0) {
      heap_insert(h,z);
    }
  }

  /* set root node */
  htree->root = node;

  return htree;
}


/*--------------------------------------------------------------------------*/
void get_codes_from_tree(TREE* htree,         /* Huffman tree */
                         long s,              /* number of symbols */
                         long* symbolmap,     /* symbols associated with leaf node ids */
                         long* codes,         /* code array */
                         long* lengths,       /* code lengths */
                         FILE* debug_file) {  /* 0 - no output, else - debug
                                              output to file */
                         
  /* extracts binary codes for s symbols from the Huffman tree, stores them
     as the corresponding integers in the decimal system */
  long i;         /* loop variable */
  long d;         /* temporary variable */
  long counter;   /* bit counter */
  TREENODE node;  /* tree node */

  if (debug_file != 0) {
    fprintf(debug_file,"\ncreating codes (s: %ld)\n",s);
  }

  /* iterate over all leaf nodes (0,...,s-1) */
  for (i=0;i<s; i++) {
    node = htree->nodes[i];
    codes[symbolmap[i]]=0;
    d=1;
    counter = 0;
    if (debug_file != 0) {
      fprintf(debug_file,"symbol %ld: ",symbolmap[i]);
    }
    while (node.parent!=0) {
      counter++;
      /* TODO: update code (left children are assigned 1, right children 0) */
      if ((((node.parent)->left)->id) == node.id) {
        codes[symbolmap[i]]+= /* TODO (HINT: use the variable d) */;
        if (debug_file != 0) {
          fprintf(debug_file,"1");
        }
      } else {
        if (debug_file != 0) {
          fprintf(debug_file,"0");
        }
      }
      d*= /* TODO */;
      node = *(node.parent);
    }
    lengths[symbolmap[i]]=counter;
    if (debug_file != 0) {
      fprintf(debug_file,"=%ld (length %ld)\n",codes[symbolmap[i]],
                                               lengths[symbolmap[i]]);
    }
  }
}

void write_node(TREENODE node,       /* node to write */
                long* symbolmap,     /* symbols associated with leaf node ids */
                BFILE* compressed,   /* compressed output file */
                FILE* debug_file) {  /* debug file */
  if ((node.left == 0) && (node.right == 0)) {
    /* leaf node: store 0 and the symbol associated with the node (1 byte) */
    if (debug_file != 0) {
      fprintf(debug_file,"node %ld: 0 (%ld)\n",node.id,symbolmap[node.id]);
    }
    set_bit(compressed,0);
    set_byte(compressed,symbolmap[node.id]);
  } else {
    /* regular node: store 1 */
    set_bit(compressed,1);
    if (debug_file != 0) {
      fprintf(debug_file,"node %ld: 1\n",node.id);
    }
    if (node.left != 0) {
      write_node(*(node.left),symbolmap,compressed,debug_file);
    }
    if (node.right != 0) {
      write_node(*(node.right),symbolmap,compressed,debug_file);
    }
  }
}

/*--------------------------------------------------------------------------*/

/* apply WNC algorithm for adaptive arithmetic integer encoding */
void encode_huffman(
  long* sourceword,   /* array containing n numbers from {0,...,s}
                         where s is the end of file symbol */
  long n,             /* length of sourceword */
  long s,             /* size of source alphabet */
  FILE* debug_file,   /* 0 - no output, else - debug output to file */
  char* compressed_filename, /* for size measurement only */
  BFILE* compressed)  {/* binary file for compressed bitstring */

  long i,j;        /* loop variables */
  long* counter;   /* array of counters for probabilities */
  long* histogram; /* histogram with all symbols with at least one occurrence */
  long* symbolmap; /* mapping of occurring symbols (1<=x<=256) to all symbols
                      (256) */
  long symbolcount; /* symbol counter */
  TREE* htree;      /* huffman tree */
  long* codes;      /* huffman codes */
  long* lengths;    /* huffman code length */
  long bcode;       /* binary code */
                  
  /* allocate memory */
  alloc_long_vector(&counter,s);
  alloc_long_vector(&symbolmap,s);
  alloc_long_vector(&histogram,s);
  alloc_long_vector(&codes,s);
  alloc_long_vector(&lengths,s);

  if (debug_file != 0) {
    fprintf(debug_file,"\nstart huffman encoding: n: %ld, s: %ld\n",n,s);
  }

  /* initialise counters */
  for (i=0;i<s;i++) {
    counter[i]=0;
  }

  /* count occurrences of each symbol in the sourceword  */

  /* TODO: count symbols in sourceword, use array "counter" */

  /* build alphabet of occurring symbols */
  symbolcount=0;
  for (i=0;i<s;i++) {
    if (counter[i]>0) {
      
      /* TODO: Assign correct values to "symbolmap" and "histogram" */

      if (debug_file != 0) {
        fprintf(debug_file,"symbol %ld (pixel value %ld): %ld\n",
                symbolcount,symbolmap[symbolcount],counter[i]);
      }
      symbolcount++;
    }
  }

  printf("alphabet contains %ld symbols\n",symbolcount);
  if (debug_file != 0) {
    fprintf(debug_file,"alphabet contains %ld symbols\n",
            symbolcount);
  } 

  /* build huffman tree */
  htree = build_huffman_tree(symbolcount,histogram,debug_file);
  
  if (debug_file != 0) {
    fprintf(debug_file,"\nstoring tree (s: %ld)\n",s);
  }

  /* recursively store all tree nodes */
  write_node(htree->root,symbolmap,compressed,debug_file);
  bfflush(compressed);
  printf("Huffman tree stored, %ld bytes overhead\n",
         get_size_of_file(compressed_filename));
  
  /* get binary codes */
  get_codes_from_tree(htree,symbolcount,symbolmap,codes,lengths,debug_file);
  
  /* store code word */
  if (debug_file != 0) {
    fprintf(debug_file,"\nstoring codeword (length: %ld)\n",n);
  }
  for (i=0;i<n;i++) {
    bcode = codes[sourceword[i]];
    if (debug_file != 0) {
      fprintf(debug_file,"symbol %ld (code %ld, %ld bits): ",sourceword[i],
              bcode,lengths[sourceword[i]]);
    }
    for (j=0;j<lengths[sourceword[i]];j++) {
      if (debug_file != 0) {
        fprintf(debug_file,"%ld",bcode%2);
      }
      /* TODO: write next bit of binary code stored in "bcode". 
         Hints:
         You can use set_bit(compressed,x) to write bits to the file (where
         x is either 0 or 1, accordingly). 
         use the operations %2 and /2 to obtain the individual bits. */
    }
    if (debug_file != 0) {
      fprintf(debug_file,"\n");
    }
  }
}

/*--------------------------------------------------------------------------*/

int main (int argc, char **args)

{
  /* user interaction */
  char   ch;                   /* for reading single chars */
  char   used[256];            /* flag for all possible chars, used to check for
                                  multiple occurences of input parameters */
  long len;
  
  /* parameters */

  /* file IO */
  char *output_file = 0;       /* file name of output image */
  char *input_file = 0;        /* file name of uncompressed image */
  char tmp_file[1000];         /* string for intermediate filenames */
  char total_file[1000];       /* file name of total compressed output */
  char *program_call;          /* call of the compression program */
  char extension[5];           /* extension of input file */
  long format;                 /* format of input file */

  /* image information */
  long nx, ny, nc;                /* image dimensions */

  /* loop variables */
  long i;

  /* image struct that contains all information
   * about original image, compressed image, and parameters */
  ImageData image;

  /* compression/decompression */
  long flag_compress=0;
  long   s = 256;             /* length of source alphabet */
  BFILE* binary_file=0;       /* binary file for compressed bitstring */
  char*  debug_file=0;        /* filename for writing debug information */
  FILE*  dfile=0;             /* file for writing debug information */
  long*  sourceword;          /* sourceword for compression 
                                 (image in vector format) */
  
  printf ("\n");
  printf ("PROGRAMMING EXERCISE P1 FOR IMAGE COMPRESSION\n\n");
  printf ("**************************************************\n\n");
  printf ("    Copyright 2018 by Pascal Peter                \n");
  printf ("    Dept. of Mathematics and Computer Science     \n");
  printf ("    Saarland University, Saarbruecken, Germany    \n\n");
  printf ("    All rights reserved. Unauthorised usage,      \n");
  printf ("    copying, hiring, and selling prohibited.      \n\n");
  printf ("    Send bug reports to                           \n");
  printf ("    peter@mia.uni-saarland.de                     \n\n");
  printf ("**************************************************\n\n");

  /* ARGUMENT PROCESSING ******************************************************/
  init_image(&image); /* initialise image with standard parameters */

  for (i = 0; i <= 255; i++) {
    used[i] = 0;
  }

  while ((ch = getopt(argc,args,"i:o:D:")) != -1) {
    used[(long)ch]++;
    if (used[(long)ch] > 1) {
      printf("Duplicate parameter: %c\n",ch);
      printf("Please check your input again.\n");
    }
    
    switch(ch) {
    case 'i': input_file = optarg;break;
    case 'o': output_file = optarg;break;
    case 'D': debug_file = optarg;break;
    default:
      printf("Unknown argument.\n");
      print_usage_message();
      return 0;
    }
  }

  if (output_file == 0 || input_file == 0) {
    printf("ERROR: Missing mandatory parameter, aborting.\n");
    print_usage_message();
    return 0;
  }


  /* prepare file names */
  sprintf(total_file,"%s.coded",output_file);

  /* create reboot string */
  len = 0;
  for (i=0;i<argc;i++) {
    len += strlen(args[i]) + 1;
  }

  program_call = (char*)malloc( sizeof(char) * ( len + 3 ) );
  sprintf(program_call," ");
  for (i=0;i<argc;i++) {
    sprintf(program_call, "%s%s ",program_call,args[i]);
  }

  /* DETERMINE COMPRESSION/DECOMPRESSION MODE**********************************/

  /* try to identify the file format via the extension */
  strcpy(extension, input_file+(strlen(input_file)-4));
  if      (!strcmp(extension, ".pgm")) format = FORMAT_PGM;
  else if (!strcmp(extension, ".ppm")) format = FORMAT_PPM;
  else {
    printf("ERROR: Extension %s not supported for input file, aborting.\n",
           extension);
    print_usage_message();
    return 0;
  }

  /* determine if we are in compression or decompression mode */
  if (format==FORMAT_PPM || format==FORMAT_PGM) {
    flag_compress = 1;
  } else {
    flag_compress = 0;
  }
  
  if (flag_compress == 1) {
    /* COMPRESS ***************************************************************/

    /* read input image */
    if (format==FORMAT_PPM) {
      read_ppm_and_allocate_memory(input_file,&image.nx,&image.ny,
                                   &image.orig_rgb);
      image.nc = 3;
      printf("Image %s loaded (PPM).\n",input_file);
    } else if (format==FORMAT_PGM) {
      read_pgm_header(input_file,&image.nx,&image.ny);
      alloc_long_cubix(&image.orig_rgb,MAXCHANNELS,image.nx+2,image.ny+2);
      read_pgm_and_allocate_memory(input_file,&image.nx,&image.ny,
                                   &image.orig_rgb[0]);
      image.nc = 1;
      printf("Image %s loaded (PGM).\n",input_file);
    }
    nx = image.nx; ny = image.ny; nc = image.nc;
    image.size_orig=get_size_of_file(input_file);
    
    /* allocate memory */
    alloc_image(&image,nx,ny);

    alloc_long_vector(&sourceword,nc*nx*ny);
    convert_image_to_vector(image.orig_rgb,1,1,nx,ny,nc,sourceword);

    /* compress with Huffman coding */
    printf("Compressing with binary Huffman Coding\n");
    sprintf(tmp_file,"%s.huf",output_file);
    binary_file = bfopen(tmp_file,"w");
    if (debug_file != 0) {
      dfile = fopen(debug_file,"w");
      fprintf(dfile,"compressing file %s\n",input_file);
    }
    encode_huffman(sourceword,nx*ny*nc,s,dfile,tmp_file,binary_file);
    bfclose(binary_file);
    printf("Resulting compression ratio: %f:1 (%ld byte)\n",
           get_compression_ratio(input_file,tmp_file),
           get_size_of_file(tmp_file));
    
  } else {
    /* DECOMPRESS *************************************************************/

  }
  
  /* ---- free memory  ---- */
  destroy_image(&image);
  free(program_call);

  return(0);
}
