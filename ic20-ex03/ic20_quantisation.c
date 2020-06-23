/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                Programming Exercise for Image Compression                */
/*                              QUANTISATION                                */ 
/*                                                                          */
/*                  (Copyright by Pascal Peter, 06/2018)                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/* global includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define SEED 0              /* random seed. set static here for the sake
                               of comparability among student's soltuions */
#define MAXQUANTISATION 255 /* maximum number of different grey values */

/* data structure for rgb colours */
typedef struct RGB RGB;
struct RGB {
  float r;
  float g;
  float b;
};


/*--------------------------------------------------------------------------*/

void alloc_vector

     (float **vector,   /* vector */
      long  n1)         /* size */

     /* allocates memory for a vector of size n1 */


{
*vector = (float *) malloc (n1 * sizeof(float));
if (*vector == NULL)
   {
   printf("alloc_vector: not enough memory available\n");
   exit(1);
   }
return;
}

/*--------------------------------------------------------------------------*/

void alloc_vector_int

     (long **vector,   /* vector */
      long  n1)         /* size */

     /* allocates memory for a vector of size n1 */


{
*vector = (long *) malloc (n1 * sizeof(long));
if (*vector == NULL)
   {
   printf("alloc_vector: not enough memory available\n");
   exit(1);
   }
return;
}

/*--------------------------------------------------------------------------*/
void alloc_vector_rgb

     (RGB **vector,   /* vector */
      long  n1)         /* size */

     /* allocates memory for a vector of size n1 */

{
*vector = (RGB *) malloc (n1 * sizeof(RGB));
if (*vector == NULL)
   {
   printf("alloc_vector: not enough memory available\n");
   exit(1);
   }
return;
}

/*--------------------------------------------------------------------------*/

void alloc_matrix

     (float ***matrix,  /* matrix */
      long  n1,         /* size in direction 1 */
      long  n2)         /* size in direction 2 */

     /* allocates memory for matrix of size n1 * n2 */


{
long i;

*matrix = (float **) malloc (n1 * sizeof(float *));
if (*matrix == NULL)
   {
   printf("alloc_matrix: not enough memory available\n");
   exit(1);
   }
for (i=0; i<n1; i++)
    {
    (*matrix)[i] = (float *) malloc (n2 * sizeof(float));
    if ((*matrix)[i] == NULL)
       {
       printf("alloc_matrix: not enough memory available\n");
       exit(1);
       }
    }
return;
}


/*--------------------------------------------------------------------------*/
void alloc_matrix_long

     (long ***matrix,  /* matrix */
      long  n1,         /* size in direction 1 */
      long  n2)         /* size in direction 2 */

     /* allocates memory for matrix of size n1 * n2 */


{
long i;

*matrix = (long **) malloc (n1 * sizeof(long *));
if (*matrix == NULL)
   {
   printf("alloc_matrix: not enough memory available\n");
   exit(1);
   }
for (i=0; i<n1; i++)
    {
    (*matrix)[i] = (long *) malloc (n2 * sizeof(long));
    if ((*matrix)[i] == NULL)
       {
       printf("alloc_matrix: not enough memory available\n");
       exit(1);
       }
    }
return;
}


/*--------------------------------------------------------------------------*/

void alloc_cubix

     (float ****cubix,  /* cubix */
      long  n1,         /* size in direction 1 */
      long  n2,         /* size in direction 2 */
      long  n3)         /* size in direction 3 */

     /* allocates memory for cubix of size n1 * n2 * n3 */


{
long i, j;

*cubix = (float ***) malloc (n1 * sizeof(float **));
if (*cubix == NULL)
   {
   printf("alloc_cubix: not enough memory available\n");
   exit(1);
   }
for (i=0; i<n1; i++)
    {
    (*cubix)[i] = (float **) malloc (n2 * sizeof(float *));
    if ((*cubix)[i] == NULL)
       {
       printf("alloc_cubix: not enough memory available\n");
       exit(1);
       }
    for (j=0; j<n2; j++)
        {
        (*cubix)[i][j] = (float *) malloc (n3 * sizeof(float));
        if ((*cubix)[i][j] == NULL)
           {
           printf("alloc_cubix: not enough memory available\n");
           exit(1);
           }
        }
    }
return;
}

/*--------------------------------------------------------------------------*/
void alloc_cubix_long

     (long ****cubix,  /* cubix */
      long  n1,         /* size in direction 1 */
      long  n2,         /* size in direction 2 */
      long  n3)         /* size in direction 3 */

     /* allocates memory for cubix of size n1 * n2 * n3 */
{
long i, j;

*cubix = (long ***) malloc (n1 * sizeof(long **));
if (*cubix == NULL)
   {
   printf("alloc_cubix: not enough memory available\n");
   exit(1);
   }
for (i=0; i<n1; i++)
    {
    (*cubix)[i] = (long **) malloc (n2 * sizeof(long *));
    if ((*cubix)[i] == NULL)
       {
       printf("alloc_cubix: not enough memory available\n");
       exit(1);
       }
    for (j=0; j<n2; j++)
        {
        (*cubix)[i][j] = (long *) malloc (n3 * sizeof(long));
        if ((*cubix)[i][j] == NULL)
           {
           printf("alloc_cubix: not enough memory available\n");
           exit(1);
           }
        }
    }
return;
}

/*--------------------------------------------------------------------------*/
void disalloc_vector

     (float *vector,    /* vector */
      long  n1)         /* size */

     /* disallocates memory for a vector of size n1 */

{
free(vector);
return;
}

/*--------------------------------------------------------------------------*/

void disalloc_vector_int

     (long *vector,    /* vector */
      long  n1)         /* size */

     /* disallocates memory for a vector of size n1 */

{
free(vector);
return;
}


/*--------------------------------------------------------------------------*/
void disalloc_vector_rgb

     (RGB *vector,    /* vector */
      long  n1)         /* size */

     /* disallocates memory for a vector of size n1 */

{
free(vector);
return;
}


/*--------------------------------------------------------------------------*/
void disalloc_matrix

     (float **matrix,   /* matrix */
      long  n1,         /* size in direction 1 */
      long  n2)         /* size in direction 2 */

     /* disallocates memory for matrix of size n1 * n2 */

{
long i;

for (i=0; i<n1; i++)
    free(matrix[i]);

free(matrix);

return;
}

/*--------------------------------------------------------------------------*/
void disalloc_cubix

     (float ***cubix,   /* cubix */
      long  n1,         /* size in direction 1 */
      long  n2,         /* size in direction 2 */
      long  n3)         /* size in direction 3 */

     /* disallocates memory for cubix of size n1 * n2 * n3 */

{
long i, j;

for (i=0; i<n1; i++)
 for (j=0; j<n2; j++)
     free(cubix[i][j]);

for (i=0; i<n1; i++)
    free(cubix[i]);

free(cubix);

return;
}

/*--------------------------------------------------------------------------*/
void disalloc_cubix_long

     (long ***cubix,   /* cubix */
      long  n1,         /* size in direction 1 */
      long  n2,         /* size in direction 2 */
      long  n3)         /* size in direction 3 */

     /* disallocates memory for cubix of size n1 * n2 * n3 */

{
long i, j;

for (i=0; i<n1; i++)
 for (j=0; j<n2; j++)
     free(cubix[i][j]);

for (i=0; i<n1; i++)
    free(cubix[i]);

free(cubix);

return;
}


/*--------------------------------------------------------------------------*/
void read_pgm_and_allocate_memory

     (const char  *file_name,    /* name of pgm file */ 
      long        *nx,           /* image size in x direction, output */
      long        *ny,           /* image size in y direction, output */
      float       ***u)          /* image, output */   

/* 
  reads a greyscale image (pgm format P5);
  allocates memory for the image u; 
  adds boundary layers of size 1 such that
  - the relevant image pixels in x direction use the indices 1,...,nx
  - the relevant image pixels in y direction use the indices 1,...,ny
*/

{
FILE   *inimage;    /* input file */
char   row[80];     /* for reading data */
long   i, j;        /* loop variables */

/* open file */
inimage = fopen (file_name, "rb");
if (NULL == inimage) 
   {
   printf ("could not open file '%s' for reading, aborting.\n", file_name);
   exit (1);
   }

/* read header */
fgets (row, 80, inimage);          /* skip format definition */
fgets (row, 80, inimage);        
while (row[0]=='#')                /* skip comments */
      fgets (row, 80, inimage);
sscanf (row, "%ld %ld", nx, ny);   /* read image size */
fgets (row, 80, inimage);          /* read maximum grey value */

/* allocate memory */
alloc_matrix (u, (*nx)+2, (*ny)+2);

/* read image data row by row */
for (j=1; j<=(*ny); j++) 
 for (i=1; i<=(*nx); i++) 
     (*u)[i][j] = (float) getc(inimage);

/* close file */
fclose(inimage);

return;

} /* read_pgm_and_allocate_memory */

/*--------------------------------------------------------------------------*/
void read_pgm_or_ppm_and_allocate_memory

     (const char  *file_name,    /* name of image file */ 
      long        *nc,           /* number of colour channels */
      long        *nx,           /* image size in x direction, output */
      long        *ny,           /* image size in y direction, output */
      float       ****u)         /* image, output */   

/* 
  reads a greyscale image (pgm format P5) or a colour image (ppm format P6);
  allocates memory for the image u; 
  adds boundary layers of size 1 such that
  - the relevant image pixels in x direction use the indices 1,...,nx
  - the relevant image pixels in y direction use the indices 1,...,ny
*/

{
FILE   *inimage;    /* input file */
char   row[80];     /* for reading data */
long   i, j, m;     /* loop variables */

/* open file */
inimage = fopen (file_name, "rb");
if (NULL == inimage) 
   {
   printf ("could not open file '%s' for reading, aborting.\n", file_name);
   exit (1);
   }

/* read header */
fgets (row, 80, inimage);                        /* image type: P5 or P6 */
if ((row[0]=='P') && (row[1]=='5'))
   *nc = 1;                                      /* P5: grey scale image */
else if ((row[0]=='P') && (row[1]=='6'))
   *nc = 3;                                      /* P6: colour image */
else
   {
   printf ("unknown image format");
   exit(0);
   }
fgets (row, 80, inimage);        
while (row[0]=='#')                /* skip comments */
      fgets (row, 80, inimage);
sscanf (row, "%ld %ld", nx, ny);   /* read image size */
fgets (row, 80, inimage);          /* read maximum grey value */

/* allocate memory */
alloc_cubix (u, (*nc), (*nx)+2, (*ny)+2);

/* read image data row by row */
for (j=1; j<=(*ny); j++) 
 for (i=1; i<=(*nx); i++) 
  for (m=0; m<=(*nc)-1; m++) 
      (*u)[m][i][j] = (float) getc(inimage);

/* close file */
fclose(inimage);

return;

} /* read_pgm_or_ppm_and_allocate_memory */

/*--------------------------------------------------------------------------*/
void comment_line

     (char* comment,       /* comment string (output) */
      char* lineformat,    /* format string for comment line */
      ...)                 /* optional arguments */

/* 
  Adds a line to the comment string comment. The string line can contain plain
  text and format characters that are compatible with sprintf.
  Example call: print_comment_line(comment,"Text %f %d",float_var,int_var);
  If no line break is supplied at the end of the input string, it is added
  automatically.
*/

{
char     line[80];
va_list  arguments;

/* get list of optional function arguments */
va_start (arguments, lineformat);

/* convert format string and arguments to plain text line string */
vsprintf (line, lineformat, arguments);

/* add line to total commentary string */
strncat (comment, line, 80);

/* add line break if input string does not end with one */
if (line[strlen(line)-1] != '\n')
   sprintf (comment, "%s\n", comment);

/* close argument list */
va_end (arguments);

return;

} /* comment_line */

/*--------------------------------------------------------------------------*/
void write_pgm

     (float  **u,          /* image, unchanged */ 
      long   nx,           /* image size in x direction */
      long   ny,           /* image size in y direction */
      char   *file_name,   /* name of pgm file */
      char   *comments)     /* comment string (set 0 for no comments) */

/* 
  writes a greyscale image into a pgm P5 file;
*/

{
FILE           *outimage;  /* output file */
long           i, j;       /* loop variables */
float          aux;        /* auxiliary variable */
unsigned char  byte;       /* for data conversion */

/* open file */
outimage = fopen (file_name, "wb");
if (NULL == outimage) 
   {
   printf("Could not open file '%s' for writing, aborting\n", file_name);
   exit(1);
   }

/* write header */
fprintf (outimage, "P5\n");                  /* format */
if (comments != 0)
   fprintf (outimage, comments);             /* comments */
fprintf (outimage, "%ld %ld\n", nx, ny);     /* image size */
fprintf (outimage, "255\n");                 /* maximal value */

/* write image data */
for (j=1; j<=ny; j++)
 for (i=1; i<=nx; i++)
     {
     aux = u[i][j] + 0.499999;    /* for correct rounding */
     if (aux < 0.0)
        byte = (unsigned char)(0.0);
     else if (aux > 255.0)
        byte = (unsigned char)(255.0);
     else
        byte = (unsigned char)(aux);
     fwrite (&byte, sizeof(unsigned char), 1, outimage);
     }

/* close file */
fclose (outimage);

return;

} /* write_pgm */

/*--------------------------------------------------------------------------*/
void write_pgm_or_ppm

     (float ***u,         /* colour image, unchanged */ 
      long  nc,           /* number of channels */
      long  nx,           /* size in x direction */
      long  ny,           /* size in y direction */
      char  *file_name,   /* name of ppm file */
      char  *comments)    /* comment string (set 0 for no comments) */

/* 
  writes an image into a pgm P5 (greyscale) or ppm P6 (colour) file;
*/

{
FILE           *outimage;  /* output file */
long           i, j, m;    /* loop variables */
float          aux;        /* auxiliary variable */
unsigned char  byte;       /* for data conversion */

/* open file */
outimage = fopen (file_name, "wb");
if (NULL == outimage) 
   {
   printf("Could not open file '%s' for writing, aborting\n", file_name);
   exit(1);
   }

/* write header */
if (nc == 1)
   fprintf (outimage, "P5\n");                  /* greyscale format */
else if (nc == 3)
   fprintf (outimage, "P6\n");                  /* colour format */
else
   {
   printf ("unsupported number of channels\n");
   exit (0);
   }
if (comments != 0)
   fprintf (outimage, comments);             /* comments */
fprintf (outimage, "%ld %ld\n", nx, ny);     /* image size */
fprintf (outimage, "255\n");                 /* maximal value */

/* write image data */
for (j=1; j<=ny; j++)
 for (i=1; i<=nx; i++)
  for (m=0; m<=nc-1; m++)
     {
     aux = u[m][i][j] + 0.499999;    /* for correct rounding */
     if (aux < 0.0)
        byte = (unsigned char)(0.0);
     else if (aux > 255.0)
        byte = (unsigned char)(255.0);
     else
        byte = (unsigned char)(aux);
     fwrite (&byte, sizeof(unsigned char), 1, outimage);
     }

/* close file */
fclose (outimage);

return;

} /* write_pgm_or_ppm */


/*----------------------------------------------------------------------------*/
float mse
(
 float*** u1,                 /* first image */
 float*** u2,                 /* second image */
 long nc,                    /* z dimension/channels */
 long nx,                    /* x dimension */
 long ny                     /* y dimension */
 )
/* computes mean square error (MSE) */
{
  long i,j,c;

  float error  = 0.0f;
  float tmp;

 

  for (c=0;c<nc;c++) {
    for (i=1; i<=nx; i++) {
      for (j=1; j<=ny; j++) {
        tmp = u1[c][i][j] - u2[c][i][j];
        error += (float)(tmp*tmp);
      }
    }
  }
  error  /= (float)(nx*ny*nc);

  return error;
} /* mse */

/*--------------------------------------------------------------------------*/
/* input: number of quantised values n, number of maximum possible grey
   value of input range (255 for 8bit). 
   output: quantisation_map_small is an array that takes as an index the
           old value and stores the corresponding coarsely quantised one.
           quanitsation_map_back is an array that takes as an index a 
           quantised value from the range [0,...,n-1]
           and assigns the corresponding value from the
           original range [0,...max_q]. */
void prepare_uniform_maps(long n, /* number of quantised values */
                          long max_q, /* maximum possible grey value */
                          long* quantisation_map_small,
                          long* quantisation_map_back) {
  long i;
  long max_levels=255; /* 16 bit */

  printf("preparing quantisation map with n=%ld and max_q=%ld\n",n, max_q); 
        
  if (n > max_levels) {
    printf("warning, quantisation exceeds max levels, set to %ld\n",max_levels);
    n = max_levels;
  }

  /* special case: 1:1 mapping */
  if ((n == 0) || (n > max_levels)) n = max_levels;
  if ((n == max_levels) && (max_q == max_levels-1)) {
      for (i = 0; i < n; i++) {
        /* TODO: set quantisation maps */
        quantisation_map_back[i] = i;
        quantisation_map_small[i] = i;
        /*
        printf("%ld -> %ld -> %ld\n",
        i,quantisation_map_small[i],quantisation_map_back[i]);
        */
      }
      return;
  }

  float delta=(float)max_q/n;
  
  /* quantisation_map_small should map the input grey value i to its 
     associated interval identifer */
  for (i = 0; i < max_q; i++) {
    /* TODO: set quantisation map small */
    quantisation_map_small[i] = i/delta;
    /*printf("%ld -> %ld\n",i,quantisation_map_small[i]);*/
  }
  /* TODO: special case for last value */
  quantisation_map_small[max_q] = n;
  /*  printf("%ld -> %ld\n",max_q,quantisation_map_small[max_q]); */

  /* quantisation_map_back should map the interval identifier i to the
     corresponding grey value */
  for (i = 0; i < n; i++) {
    /* TODO: set quantisation map back */
    quantisation_map_back[i] = ((float)i+0.5)*delta;
    /*printf("%ld -> %ld\n",i,quantisation_map_back[i]);*/
  }

}

/*--------------------------------------------------------------------------*/
/* uses given quantisation maps to perform a full quantisation, i.e.
   the encoder part (mapping to the new intervals) and the decoder part (mapping
   back to the quantised value of the original image range */
void quantise_image_scalar(float*** original,
                           long nx, long ny,
                           long sc, long nc,
                           long max_q,
                           long n,
                           float*** quantised) {
  long quantisation_map_small[256];
  long quantisation_map_back[256];

  long x, y, c;
  long bx = 1, by = 1;
  
  prepare_uniform_maps(n,max_q,quantisation_map_small,quantisation_map_back);

  for (c = sc; c < nc; c++)
    for (x = bx; x < nx + bx; x++)
      for (y = by; y < ny + by; y++) {
        if (original[c][x][y]>255) {
          original[c][x][y]=255;
        }
        if (original[c][x][y]<0) {
          original[c][x][y]=0;
        }
        /* TODO: set quantised image value */
        quantised[c][x][y] = quantisation_map_small[(int)(original[c][x][y])];
      }
}

/* -------------------------------------------------------------------------- */
/* computes histogram of colours in given image */
long compute_histogram(float*** image, long nc, long nx, long ny, long*** hist)
{
  long i,j,c;
  long count;

  /* initialise histogram */
  for (c=0;c<=255;c++) {
    for (i=0;i<=255;i++) {
      for (j=0;j<=255;j++) {
        hist[c][i][j]=0;
      }
    }
  }
  
  /* count colour occurrences */
  count=0;
  for (i=1;i<=nx;i++) {
    for (j=1;j<=ny;j++) {
      if (hist[(long)image[0][i][j]][(long)image[1][i][j]][(long)image[2][i][j]]
          ==0) {
        count++;
      }
      hist[(long)image[0][i][j]][(long)image[1][i][j]][(long)image[2][i][j]]++;
    }
  }
  return count;
}

/* -------------------------------------------------------------------------- */
/* computes euclidean distance between input RGB colours  */
float compute_distance(RGB x, RGB y) {
  return sqrtf((x.r-y.r)*(x.r-y.r)+(x.g-y.g)*(x.g-y.g)+(x.b-y.b)*(x.b-y.b));
}

/* -------------------------------------------------------------------------- */
/* for given input colour, find closest cluster centre among clusters  */ 
long find_closest_cluster(RGB colour, RGB* centres, long cluster_count) {
  long i; /* loop variable */
  long min_index=0;
  float distance, min_distance=256*256+256*256+256*256;

  for (i=0; i<cluster_count; i++) {
    /* TODO: find closest cluster centre and store it in min_index.
       Hint: You may use the function compute_distance */
    distance=compute_distance(colour,centres[i]);
    if(distance<min_distance){
      min_distance=distance;
      min_index=i;
    }
  }
  return min_index;
}

/*--------------------------------------------------------------------------*/
/* quantise input RGB image "original" to q different colours with k-means 
   clustering and store it in "quantised" */
void quantise_image_kmeans(float*** original,
                           long nx, long ny,
                           long n,
                           float*** quantised) {

  long i,j,k;
  long cindex;
  long changes=1;
  RGB* centres;
  RGB* colours;
  long colour_count;
  long c;
  long* quantisation_map;
  RGB sum;
  long count;
  long iterations=0;
  long*** hist;
  long*** index_map;

  alloc_cubix_long(&hist,256,256,256);
  alloc_cubix_long(&index_map,256,256,256);
  
  /* get colours: compute histogram, assign the different colours from the
  original image to the RGB-array colours (index 0,...,colour_count-1). */
  colour_count = compute_histogram(original, 3, nx, ny, (long***)hist);

  printf("Found %ld different colours in %ld pixels.\n",colour_count,nx*ny);

  /* allocate memory now that number of colours is known */
  alloc_vector_rgb(&colours,colour_count);
  alloc_vector_int(&quantisation_map,colour_count);

  /* store colour palette in array "colours", create map from RGB to the
     index of this array in "index_map" for later access */
  count=0;
  for (i = 0; i <= 255; i++) {
    for (j = 0; j <= 255; j++) {
      for (k = 0; k <= 255; k++) {
        if (hist[i][j][k]>0) {
          colours[count].r = (float)i;
          colours[count].g = (float)j;
          colours[count].b = (float)k;
          index_map[i][j][k]=count;
          /* printf("colour %ld: (%f %f %f)\n",
             colour_count,
             colours[colour_count].r, colours[colour_count].g,
             colours[colour_count].b);*/
          count++;
        }
      }
    }
  }

  /* allocate memory for centres */
  alloc_vector_rgb(&centres,colour_count); // Maybe only n and not colour_count?
  
  /* intialise q cluster centres randomly */
  srand ((unsigned int) SEED);
  for (i=0; i<n; i++) {
    /* TODO: create random variable in the range [0,255]
       Hint: Have a look how the function rand() works.
    */
    centres[i].r = rand()%256;
    centres[i].g = rand()%256;
    centres[i].b = rand()%256;
    /*printf("centre %ld: (%f, %f, %f)\n",i,centres[i].r,centres[i].g,centres[i].b);*/
  }

  /* initialise quantisation map */
  for (i=0; i<colour_count; i++) {
    quantisation_map[i] = -1; /* unmapped so far */
  }
  
  /* iterate */
  while (changes > 0) {
    iterations++;
    changes = 0;

    /* TODO: associate each colour with closest cluster, record number
       of actual changes. */
    /* Hint: Use the function find_closest_cluster and the variables
       quantisation_map and changes */ 
    for (i=0; i<colour_count; i++) {
      long new_centre=find_closest_cluster(colours[i],centres,n);
      if(new_centre!=quantisation_map[i]){
        quantisation_map[i]=new_centre;
        changes++;
      }
      /*
      printf("(%f %f %f) closest cluster: %ld, former mapping %ld\n",
             colours[i].r,colours[i].g,colours[i].b,c,quantisation_map[i]);
      */
    }

    /* TODO: update cluster centres */
    for (i=0; i<n; i++) {
      /* TODO: sum up all colours in cluster */
      sum.r = 0; sum.g = 0; sum.b = 0; count = 0;
      for (j=0; j<colour_count; j++) {
        if (quantisation_map[j] == i) {
          /* printf("%ld: qmap %ld, (%f, %f, %f)\n",i,j,colours[i].r,
             colours[i].g,colours[i].b);*/
          sum.r+=colours[j].r;
          sum.g+=colours[j].g;
          sum.b+=colours[j].b;
          count++;
        }
      }
      /* TODO: compute new mean taking account number of points in cluster */
      if (count > 0) {
        centres[i].r = sum.r/count;
        centres[i].g = sum.g/count;
        centres[i].b = sum.b/count;
      }
      /* printf("centre %ld: (%f, %f, %f)\n",
         i,centres[i].r,centres[i].g,centres[i].b); */
    }
    printf("Iteration %ld, %ld changes\n",iterations,changes);
  }

  /* convert cluster centres to values in integer range {0,1,...,255} */
  for (i=0; i<n; i++) {
    centres[i].r=roundf(centres[i].r);
    if (centres[i].r>255) centres[i].r = 255;
    centres[i].g=roundf(centres[i].g);
    if (centres[i].g>255) centres[i].g = 255;
    centres[i].b=roundf(centres[i].b);
    if (centres[i].b>255) centres[i].b = 255;
  }

  /* TODO: assign quantised colours to image */
  for (i = 1; i <= nx; i++) {
    for (j = 1; j <= ny; j++) {
      /* HINT: the first index of quantised corresponds to the colour
         0 ~ R, 1 ~ G, 2 ~ B. You can use the variable cindex to recover
         the correct cluster index from the index_map array (See beginning
         of this function). */
        cindex=index_map
          [(int)(original[0][i][j])]
          [(int)(original[1][i][j])]
          [(int)(original[2][i][j])];
        // for (size_t c = 0; c < 3; c++)
        // {
          cindex=quantisation_map[cindex];
        // }
        
         quantised[0][i][j]=centres[cindex].r;
         quantised[1][i][j]=centres[cindex].g;
         quantised[2][i][j]=centres[cindex].b;
      /*
      printf("%ld -> %ld -> (%ld %ld %ld)\n",cindex,
             quantisation_map[cindex],
             (long)quantised[0][i][j],(long)quantised[1][i][j],
             (long)quantised[2][i][j]);*/
    }
  }

  /* free memory */
  disalloc_vector_rgb(colours,colour_count);
  disalloc_vector_rgb(centres,n);
  disalloc_vector_int(quantisation_map,colour_count);
  disalloc_cubix_long(hist,256,256,256);
  disalloc_cubix_long(index_map,256,256,256);

}


/*--------------------------------------------------------------------------*/
void print_usage_message() {
  printf("./ic18_quantisation -i input_file -o output_prefix\n");
  printf("list of mandatory paramters:\n");
  printf("-i input_file    (string): uncompressed image, e.g. \"image.ppm\"\n");
  printf("-o out_prefix    (string): prefix for output files, e.g. \"my_image\"\n");
  printf("-n quantised values (int): number of coarsely quantised values (1,...,256)\n");
  printf("-k                       : flag that activates k-means clustering\n");
}

/*--------------------------------------------------------------------------*/

int main (int argc, char **args)

{
  /* user interaction */
  char   ch;                   /* for reading single chars */
  char   used[256];            /* flag for all possible chars, used to check for
                                  multiple occurences of input parameters */
  /* file IO */
  char *output_file = 0;       /* file name of output image */
  char *original_file = 0;     /* file name of uncompressed image */
  char tmp_file[1000];         /* string for intermediate filenames */
  char comments[10000];        /* string for comments */
  char *program_call;          /* call of the compression program */
  long len;

  /* auxiliary variables */
  long i;

  /* parameters */
  long n=32; /* number of quantised grey or colour levels */
  long quantisation_type = 0; /* 0 - uniform scalar, else: kmeans vector */

  /* images */
  float ***original=0;         /* input image */
  float ***quantised=0;        /* quantised */
  long  nx, ny;                /* image size */
  long  nc;                    /* number of channels */

  /* error measurement */
  float error;  
  
  printf ("\n");
  printf ("PROGRAMMING EXERCISE FOR IMAGE COMPRESSION: QUANTISATION \n\n");
  printf ("********************************************************\n\n");
  printf ("    Copyright 2018 by Pascal Peter                \n");
  printf ("    Dept. of Mathematics and Computer Science     \n");
  printf ("    Saarland University, Saarbruecken, Germany    \n\n");
  printf ("    All rights reserved. Unauthorised usage,      \n");
  printf ("    copying, hiring, and selling prohibited.      \n\n");
  printf ("    Send bug reports to                           \n");
  printf ("    peter@mia.uni-saarland.de                     \n\n");
  printf ("********************************************************\n\n");

  /* ARGUMENT PROCESSING ******************************************************/
  for (i = 0; i <= 255; i++) {
    used[i] = 0;
  }

  while ((ch = getopt(argc,args,"i:o:n:k")) != -1) {
    used[(long)ch]++;
    if (used[(long)ch] > 1) {
      printf("Duplicate parameter: %c\n",ch);
      printf("Please check your input again.\n");
    }

    switch(ch) {
    case 'i': original_file = optarg;break;
    case 'o': output_file = optarg;break;
    case 'n': n = atoi(optarg);break;
    case 'k': quantisation_type = 1; break;
    default:
      printf("Unknown argument.\n");
      print_usage_message();
      return 0;
    }
  }

  /* check validity of parameter input */
  if (output_file == 0 || original_file == 0) {
    printf("ERROR: missing mandatory parameter\n");
    print_usage_message();
    return 0;
  }

  /* create call string */
  len = 0;
  for (i=0;i<argc;i++) {
    len += strlen(args[i]) + 1;
  }

  program_call = (char*)malloc( sizeof(char) * ( len + 3 ) );
  sprintf(program_call," ");
  for (i=0;i<argc;i++) {
    sprintf(program_call, "%s%s ",program_call,args[i]);
  }
      
  /* READ INPUT DATA AND ALLOCATE MEMORY **************************************/

  /* read input image */
  read_pgm_or_ppm_and_allocate_memory(original_file, &nc, &nx, &ny, &original);
  alloc_cubix(&quantised,nc,nx+2,ny+2);
  
  /* QUANTISE *****************************************************************/

  if (quantisation_type == 0) {
    quantise_image_scalar(original,nx,ny,0,nc,MAXQUANTISATION,n,quantised);
  } else {
    quantise_image_kmeans(original,nx,ny,n,quantised);
  }

  error = mse(original,quantised,nc,nx,ny);
  printf("Quantisation error (MSE): %f\n",error);
  
  /* OUTPUT *******************************************************************/
  comments[0] = '\0';
  comment_line (comments, (char*)"# QUANTISATION\n");
  comment_line (comments, (char*)"# quantisation type: %ld\n",quantisation_type);
  comment_line (comments, (char*)"# quantisation levels: %ld\n",n);
  comment_line (comments, (char*)"# initial image:  %s\n", original_file);
  comment_line (comments, (char*)"# MSE: %f\n", error);

  if (nc>1) {
    sprintf(tmp_file, "%s_quantised.ppm",output_file);
  } else {
    sprintf(tmp_file, "%s_quantised.pgm",output_file);
  }
  write_pgm_or_ppm (quantised, nc, nx, ny, tmp_file, comments);
  
  disalloc_cubix(original,nc,nx+2,ny+2);
  disalloc_cubix(quantised,nc,nx+2,ny+2);

  return(0);
}
