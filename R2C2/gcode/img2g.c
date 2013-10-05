#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <time.h>       /* time_t, struct tm, time, localtime, asctime */


// image datatype
typedef struct Image
{
  int w,h,d,p,m; // width, height, depth (bytes/pixel), pitch (bytes from start of line n to start of line n+1)
  unsigned char *b; // buffer
} Image;

int fast = 2000, slow=600; 
  
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
    // printf("; \n%d '%s'\n", state, line);
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
        printf("; Image: %dx%d pixels\n", img->w, img->h);
        state = 2;
        break;
      case 2:
        sscanf(line, "%d", &img->m);
        printf("; Image: max value=%d\n", img->m);
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
  printf("; len=%d\n", len);
  printf("M703\n");

  while ( len )
  {
    printf("\nM702 x");
    for(i=0; i<35 && len; i++, len--)
      printf("%02X", (int)*d++);
  }
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
  int bit, len;
  Image *img;
  unsigned char line[100000], *l; 
  
  time_t rawtime;
  struct tm * timeinfo;

  double dy=NOZZLES * (INCH/DPI), dx, de, py=0, pe=0;
 
  printf("; Open %s\n", argv[1]);
  img = pgm_open(argv[1]);
  printf("; Dump...\n");
  // dump(img);
 
  dx = (INCH/DPI) * img->w;
  de = img->w / PPI;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
   
  printf(
   "\n; Converted file, using img2g, from '%s' (%dx%d pixels)\n"
   "; Converted on %s\n"
   "G21 ; millimeters\n"
   "G92 X0 Y0 E0 Z0; zero\n"
   "G90; absolute positions\n"
   "M200 E1200 ; 1200 pulses/mm (nore: we need 12 pulses per x-pixel)\n"
   "M700 S4095  ; enable all nozzles\n"
   "M701 S10  ; pulse duration 5usec\n"
   "G1 F600\n",
   argv[1], img->w, img->h,  asctime (timeinfo)
  );
 
 
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
    printf("\nG4\n");
    printf("G1 F%d X0 Y%f\n", fast, (float)py); 
    printf("G1 F%d X0 Y%f\nG4\n", fast, (float)py); 
    printf("G1 F%d X0 Y%f\n", fast, (float)py); 
    dump_hex(line, len);
    pe += de;
    printf("\nG4\nG1 F%d X%f Y%f E%f\n", slow, (float)dx, (float)py, (float)pe); 
    printf("G1 F%d X%f Y%f E%f\n", slow, (float)dx, (float)py, (float)pe); 
    printf("G1 F%d X%f Y%f E%f\nG4\n", slow, (float)dx, (float)py, (float)pe); 
    py += dy;

  }

  printf("G1 F%d X0 Y0\nG4\n", fast); 
  printf("G1 F%d X0 Y0\nG4\n", fast); 
  
}