#include <stdio.h>
#include <malloc.h>
#include <memory.h>

// image datatype
typedef struct Image
{
  int w,h,d,p,m; // width, height, depth (bytes/pixel), pitch (bytes from start of line n to start of line n+1)
  unsigned char *b; // buffer
} Image;


/**
*** Open PGM file
**/
Image *pgm_open(char *name)
{
  Image *img;
  FILE *fp = fopen(name, "rb");
  char line[256];
  int state = 0;
  
  if ( fp == NULL ) return NULL;
  img = malloc(sizeof(Image));
  while(state != 10)
  {
    fgets(line, sizeof(line)-1, fp);
    printf("\n%d '%s'\n", state, line);
    if ( line[0] == '#' ) 
      continue;
    switch ( state )
    {
      case 0: 
        if (line[0] != 'P' && line[1] != '5' ) 
        {
          printf("Error '%s' is not a PGM file!\n", name);
          state = 10;
        } else
          state = 1;
        break;
      case 1:
        sscanf(line, "%d %d", &img->w, &img->h);
        printf("Image: %dx%d pixels\n", img->w, img->h);
        state = 2;
        break;
      case 2:
        sscanf(line, "%d", &img->m);
        printf("Image: max value=%d\n", img->m);
        img->d = (img->m > 255 ? 2 : 1);
        img->p = img->w * img->d;
        img->b = malloc(img->w*img->h*img->d);
        fread(img->b, img->w * img->h * img->d, 1, fp);
        state = 10;
        break;
    }    
  }
  fclose(fp);
  return img;
}


/**
*** dump image to console
**/
void dump(Image *img)
{
  printf("Image: %dx%d (%d bytes/pixel), max=%d\n", img->w, img->h, img->d, img->m);
  for(int y=0; y<img->h; y++)
  {
    printf("\n");
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
  for(int i=0; i<len; i++)
    printf("%02X", (int)*d++);
}


// defines
#define NOZZLES 12 // nr of nozzles
#define DPI 96.0  // Dots per inch
#define INCH 25.4 // mm per 1 inch 
#define PPI 100.0 // pulses per inch (e coordinate)


/**
*** main
**/
int main(int argc, char *argv[])
{
  int x,y,bit=0;
  Image *img;
  unsigned char line[10000], *l,len; 
 
  double dy=NOZZLES * (INCH/DPI), dx, de, py=0, pe=0;
 
  printf("Open %s\n", argv[1]);
  img = pgm_open(argv[1]);
  printf("Dump...\n");
  dump(img);
 
  dx = (INCH/DPI) * img->w;
  de = img->w / PPI;
 
  // write all lines
  for(int y=0; y<img->h; y+=NOZZLES)
  {
    l = line;
    len = 0;
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
   
    printf("\n");
    printf("G1 X0 Y%f\n", (float)py); 
    dump_hex(line, len);
    pe += de;
    printf("\nG1 X%f Y%f E%f\n", (float)dx, (float)py, (float)pe); 
    py += dy;
    printf("\n");

  }
  
}