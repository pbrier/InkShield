/* Copyright (C) 2013 Peter Brier   */
/* All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <time.h>       /* time_t, struct tm, time, localtime, asctime */



// defines
#define NOZZLES (12) // nr of nozzles
#define DPI (96.0) // native dpi (X and Y)
#define INCH (25.4) // mm per 1 inch 

#define COPYRIGHT "img2g -- convert PGM image files to GCODE - (c) 2013 - Peter Brier\n"


// image datatype
typedef struct Image
{
  int w,h,d,p,m; // width, height, depth (bytes/pixel), pitch (bytes from start of line n to start of line n+1)
  unsigned char *b; // buffer
} Image;


// global data
int fast=1200, slow=600; 
int x_dpi=DPI, y_dpi=DPI;
FILE *fout=NULL;
  
/**
*** Open PGM file
**/
Image *pgm_open(char *name)
{
  Image *img=NULL;
  FILE *fp = stdin;
  char line[256];
  int state = 0;

  if ( name != NULL ) 
    fp = fopen(name, "rb");
  
  if ( fp == NULL ) return NULL;
  img = malloc(sizeof(Image));
  while(state != 10)
  {
    fgets(line, sizeof(line)-1, fp);
    // printf("; \n%d '%s'\n", state, line);
    if ( line[0] == '#' ) 
      continue;
    switch ( state )
    {
      case 0: 
        if (line[0] != 'P' && line[1] != '5' ) 
        {
          fprintf(fout, "Error '%s' is not a PGM file!\n", name);
          state = 10;
        } else
          state = 1;
        break;
      case 1:
        sscanf(line, "%d %d", &img->w, &img->h);
        fprintf(fout, "; Image: %dx%d pixels\n", img->w, img->h);
        state = 2;
        break;
      case 2:
        sscanf(line, "%d", &img->m);
        fprintf(fout,"; Image: max value=%d\n", img->m);
        img->d = (img->m > 255 ? 2 : 1);
        img->p = img->w * img->d;
        img->b = malloc(img->w*img->h*img->d);
        fread(img->b, img->w * img->h * img->d, 1, fp);
        state = 10;
        break;
    }    
  }
  fclose(fp);
  if ( state == 10 )
  {
    free(img);
    img = NULL;
  }
  return img;
}


/**
*** dump image to console
**/
void dump(Image *img)
{
  printf("; Image: %dx%d (%d bytes/pixel), max=%d\n", img->w, img->h, img->d, img->m);
  for(int y=0; y<img->h; y++)
  {
    printf("\n; ");
    for(int x=0; x<img->w; x++)
    {
      if ( *(img->b + img->p * y + x) == 0xFF )
        putchar('*');
      else
        putchar('.');
    }
  }
}


/**
*** Dump buffer as HEX string
**/
void dump_hex(unsigned char *d, int len)
{
  int i = -1;
  fprintf(fout, "; len=%d\nM703\n", len);

  while ( len )
  {
    fprintf(fout, "\nM702 x");
    for(i=0; i<35 && len; i++, len--)
      fprintf(fout, "%02X", (int)*d++);
  }
}



/**
*** main
**/
int main(int argc, char *argv[])
{
  int bit, len;
  Image *img;
  unsigned char line[100000], *l; 
  time_t rawtime;
  struct tm * timeinfo;
  double dx, dy, de, py=0, pe=0;
  char *in=NULL, *out=NULL;

 
  for(int i=1; i<argc; i++)
  {
    if (strncmp(argv[i], "-xdpi=", 6) == 0 ) sscanf(argv[i]+6, "%d", &x_dpi);
    else if (strncmp(argv[i], "-ydpi=", 6) == 0 ) sscanf(argv[i]+6, "%d", &y_dpi);
    else if (strncmp(argv[i], "-slow=", 6) == 0 ) sscanf(argv[i]+6, "%d", &slow);
    else if (strncmp(argv[i], "-fast=", 6) == 0 ) sscanf(argv[i]+6, "%d", &fast);
    else if (strncmp(argv[i], "-in=", 4) == 0 ) in = argv[i] + 4;
    else if (strncmp(argv[i], "-out=", 5) == 0 ) out = argv[i] + 5;
    else
    {
      if ( in == NULL ) 
        in = argv[i];
      else
        out = argv[i];
    }
  }
  
  if ( out == NULL )
  {  
    fout = stdout;
    out = "(stdout)";    
  } else
    fout = fopen(out, "w");
 
  fprintf(fout, "; Open '%s'\n", ( in == NULL ? "(stdin)" : in));
  img = pgm_open( in );
  if ( img == NULL) 
  {
    fprintf(stderr,
     COPYRIGHT
     "No image specified!\n\nUse img2g.exe [parameters] [infile.pgm] [outfile.gcode]\n"
     "Parameters: \n"
     "  -xdpi=[x-resolution]    X-resolution\n"
     "  -ydpi=[y-resolution]    Y-resolution\n"
     "  -fast=[fast move speed] fast movement speed (mm/min)\n"
     "  -slow=[slow move speed] slow (printing) speed (mm/min)\n"
     "  -in=[filename]          Input filename (use stdin if not specified)\n"
     "  -out=[filename]         Output filename (use stdout if not specified)\n"
     "The input file needs to be a 8bits/pixel grayscale binary PGM file (portable gray map)\n"
    );
    return -1;
  }
  //printf("; Dump...\n");
  // dump(img);
 
  dx = (INCH/x_dpi) * img->w;
  dy = NOZZLES * (INCH/y_dpi);
  de = img->w / DPI;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  
  fprintf(fout,
   "\n; " COPYRIGHT "\n"
   "\n; Converted file, using img2g, from '%s' (%dx%d pixels)\n"
   "; Converted on %s\n"
   "G21 ; millimeters\n"
   "G92 X0 Y0 E0 Z0; zero\n"
   "G90; absolute positions\n"
   "M200 E%f ; note: we need 'n-nozzle' pulses per x-pixel\n"
   "M700 S4095  ; enable all nozzles\n"
   "M701 S10  ; pulse duration 5usec\n"
   "G1 F600\n",
   argv[1], img->w, img->h,  asctime (timeinfo), (float)(NOZZLES * DPI)
  );
  fprintf(fout, "; fast=%d, slow=%d, xdpi=%d, ydpi=%d\n", fast, slow, x_dpi, y_dpi);
 
  // write all lines
  for(int y=0; y<img->h; y+=NOZZLES)
  {
    l = line;
    len = 0;
    bit = 0;
    memset(line, 0, sizeof(line));
    for(int x=0; x < img->w; x++)
    {
      for(int yy=0; yy<NOZZLES; yy++)
      {
        if ( y+yy < img->h )
        {
          if ( *(img->b + img->p*(y+yy) + x) != 0xFF )
            *l |= (1<<bit);
        }            
        if ( ++bit > 7 )
        {
          bit=0;
          l++;
          len++;
        }
      }
    }
   
   // output the lines
    fprintf(fout, "\nG4\nG1 F%d X0 Y%f\nG4\n", fast, (float)py); 
       fprintf(fout, "\nG4\nG1 F%d X0 Y%f\nG4\n", fast, (float)py); 
    dump_hex(line, len);
    pe += de;
    fprintf(fout,"\nG4\nG1 F%d X%f Y%f E%f\nG4\n", slow, (float)dx, (float)py, (float)pe); 
    fprintf(fout,"\nG4\nG1 F%d X%f Y%f E%f\nG4\n", slow, (float)dx, (float)py, (float)pe); 
    fprintf(fout,"\nG4\nG1 F%d X%f Y%f E%f\nG4\n", slow, (float)dx, (float)py, (float)pe); 
    
    py += dy;

  }

  fprintf(fout, "G1 F%d X0 Y0\nG4\n", fast); 
  
}