#include <getopt.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                Programming Exercise for Image Compression                */
/*                              INPAINTING                                  */ 
/*                                                                          */
/*                  (Copyright by Pascal Peter, 06/2018)                    */
/*                Partially based on code by Joachim Weickert               */
/*                                                                          */
/*--------------------------------------------------------------------------*/

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

void read_pgm_and_allocate_memory

     (const char  *file_name,    /* name of pgm file */ 
      long        *nx,           /* image size in x direction, output */
      long        *ny,           /* image size in y direction, output */
      float       ***u)          /* image, output */   

/* 
  reads a greyscale image that has been encoded in pgm format P5;
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
void write_pgm

     (float  **u,          /* image, unchanged */ 
      long   nx,           /* image size in x direction */
      long   ny,           /* image size in y direction */
      char   *file_name,   /* name of pgm file */
      char   *comments)    /* comment string (set 0 for no comments) */

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

void comment_line

     (char* comment,       /* comment string (output) */
      char* lineformat,    /* format string for comment line */
      ...)                 /* optional arguments */

/* 
  Add a line to the comment string comment. The string line can contain plain
  text and format characters that are compatible with sprintf.
  Example call: print_comment_line(comment,"Text %f %ld",float_var,int_var);
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

void dummies
 
     (float **u,        /* image matrix */
      long  nx,         /* size in x direction */
      long  ny)         /* size in y direction */

/* creates dummy boundaries by mirroring */

{
long i, j;  /* loop variables */

for (i=1; i<=nx; i++)
    {
    u[i][0]    = u[i][1];
    u[i][ny+1] = u[i][ny];
    }

for (j=0; j<=ny+1; j++)
    {
    u[0][j]    = u[1][j];
    u[nx+1][j] = u[nx][j];
    }
return;
}  

/*--------------------------------------------------------------------------*/

void inpaint

     (long     nx,        /* image dimension in x direction */ 
      long     ny,        /* image dimension in y direction */
      float    ht,        /* time step size, 0 < ht <= 0.25 */
      float    hx,        /* pixel size in x direction */
      float    hy,        /* pixel size in y direction */
      long     iterations,/* number of explicit diffusion steps */
      float    **mask,    /* inpainting mask (<0.5 for unknown data) */
      float    **f,       /*  original image */
      float    **u)       /* output: inpainted */

/* 
 linear diffusion subroutine for du/dt = Laplace(u) on non-mask pixels
*/

{
  long    i, j, k;       /* loop variables */
  float   rx, ry;     /* mesh ratios */
  float   avg;
  long mask_pixels;
     
  /* ---- generate initialisation  ---- */
  /* set known data at mask locations, initialise with average grey value
     of known data everywhere else */

  /* TODO */
  for (size_t j = 0; j < ny; j++)
  {
    for (size_t i = 0; i < nx; i++)
    {
      avg+=f[i][j];
    }
  }
  avg/=nx*ny;
  for (size_t j = 0; j < ny; j++)
  {
    for (size_t i = 0; i < nx; i++)
    {
      if(mask[i][j]>=0.5)
        u[i+1][j+1]=f[i][j];
      else
        u[i+1][j+1]=avg;
    }
  }
  
  float** tmp=0;
  alloc_matrix(&tmp, nx+2, ny+2);

  rx = 1.0 / (hx * hx);
  ry = 1.0 / (hy * hy);
  for (k=0;k<iterations;k++) {
    /* ---- create dummy boundaries for u by mirroring ---- */
    dummies (u, nx, ny);

    /* ---- diffusive averaging ---- */

    /* TODO: In each iteration, perform one explicit diffusion step with
       the discretisation from the lecture */
    for (size_t j = 0; j <= ny+1; j++)
    {
      for (size_t i = 0; i <= nx+1; i++)
      {
        tmp[i][j]=u[i][j];
      }
    }
    for (size_t j = 1; j <= ny; j++)
    {
      for (size_t i = 1; i <= nx; i++)
      {
        if(mask[i-1][j-1]<0.5)
          u[i][j]=
            ht*rx*tmp[i+1][j]+
            ht*rx*tmp[i-1][j]+
            (1.0-2*ht*rx-2*ht*ry)*tmp[i][j] +
            ht*ry*tmp[i][j+1] + 
            ht*ry*tmp[i][j-1];
      }
    }

  }

  disalloc_matrix(tmp, nx+2, ny+2);

  return;
}

/*--------------------------------------------------------------------------*/
float mse(float **u, float **f, long nx, long ny) {
  long i,j;
  long sum, diff;

  sum=0;
  for (i=1;i<=nx;i++)
    for (j=1;j<=ny;j++) {
      diff = u[i][j]-f[i][j];
      sum += diff*diff;
    }
  
  return (double)sum/(double)(nx*ny);
}


/*--------------------------------------------------------------------------*/

int main(int argc, char** args) {
  int ch;	  /* all-purpose char */
  char used[256]; /* flag for all possible chars, used to check for multiple
                   * occurences of input parameters */
  long nx=256,ny=256; /* image dimensions */
  float** mask=0;
  float** image=0;
  float** original=0;
  char* image_file=0, *mask_file = 0, *output_file = 0;
  long bx =1, by =1;
  char comments[256];
  long i,j;
  long iterations = 1000;
  
  comments[0]='\0';
  
  /* process input parameters */
  for (ch = 0;ch <= 255; ch++)
    used[ch] = 0;
  while ((ch = getopt(argc,args,"i:m:o:s:k:")) != -1) {
    used[ch]++;

    if (used[ch] > 1) {
      printf("Double parameter: %c\n",ch);
      printf("Please check your input again.\n");
    }

    switch(ch) {
      case 'i': image_file = optarg;break;
      case 'm': mask_file = optarg;break;
      case 'o': output_file = optarg;break;
      case 'k': iterations = atoi(optarg);break;
      default: printf("Unknown argument.\n");
    }
  }

  if ((mask_file==0) || (image_file==0) || (output_file==0)) {
    printf("usage: -i original_image.pgm -m mask.pgm -o output_image.pgm\n");
    return 0;
  }
  
  /* load images */
  if (image_file != 0) {
    read_pgm_and_allocate_memory (image_file, &nx, &ny, &original);
  }
  alloc_matrix(&image, nx+2*bx, ny+2*by);
  if (mask_file != 0) {
    read_pgm_and_allocate_memory (mask_file, &nx, &ny, &mask);
  }

  for (j=by; j<ny+by; j++)
    for (i=bx; i<nx+bx; i++) {
      if (mask[i][j]>0.5) {
        mask[i][j]=1.0;
      } else {
        mask[i][j]=0.0;
      }
    }

  /* inpaint */
  inpaint(nx,ny,0.249,1.0,1.0,iterations,mask,original,image);
  printf("MSE after %ld iterations: %f\n",iterations,mse(image,original,nx,ny));

  write_pgm(image,nx,ny,output_file,comments);
  printf ("output mask %s successfully written\n\n", mask_file);
  disalloc_matrix(mask, nx+2*bx, ny+2*by);
  disalloc_matrix(image, nx+2*bx, ny+2*by);
  disalloc_matrix(original, nx+2*bx, ny+2*by);
  
  return 0;
}
