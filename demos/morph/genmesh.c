/***************************************************************
*                                                              *
*      Copyright (C) 1997 by Sony Computer Entertainment       *
*                       All rights Reserved                    *
*                                                              *
*                      S. Ashley 8th May 97                    *
*															   *
***************************************************************/

// **** this is a dos program for generating polygon meshes ****
// **** for use in the PSX morphing program ****

#include <stdio.h>
#include <math.h>

#define PI 3.14159265359
#define xgrid 20		  // grid width
#define ygrid 16		  // grid height
#define step 32			  // polygon size

void main(void)
  {
  FILE *f;
  int i,j;
  int x,y,z;
  int c;
  double segx,segy;
  int n;

  segx = 2 * PI / xgrid;			 // circle split into xgrid segments
  segy = 2 * PI / ygrid;			 // circle split into ygrid segments
  n = 3 * (xgrid + 1) * (ygrid + 1); // number of element in array

  // **** make flat mesh ****
  f = fopen("flat.h","w");

  fprintf(f,"short flat[%d] = {\n",n);

  c = 0;
  for(j = 0; j <= ygrid; j++)
    for (i = 0; i <= xgrid; i++)
      {
      x = (i - xgrid / 2) * step;
      y = (j - ygrid / 2) * step;
      fprintf(f,"%4d, %4d,    0, ",x,y);

      if ((++c % 4) == 0)
        fprintf(f,"\n");
      }

  fprintf(f,"\n};");

  fclose(f);

  // **** make cylinder mesh ****
  f = fopen("cylinder.h","w");

  fprintf(f,"short cylinder[%d] = {\n",n);

  c = 0;
  for(j = 0; j <= ygrid; j++)
    for (i = 0; i <= xgrid; i++)
      {
      x = (int)(-360 * sin(i * segx));
      y = (j - ygrid / 2) * step;
      z = (int)(360 * cos(i * segx));

      fprintf(f,"%4d, %4d, %4d, ",x,y,z);

      if ((++c % 4) == 0)
        fprintf(f,"\n");
      }

  fprintf(f,"\n};");

  fclose(f);

  // **** make sphere mesh ****
  f = fopen("sphere.h","w");

  fprintf(f,"short sphere[%d] = {\n",n);

  c = 0;
  for(j = 0; j <= ygrid; j++)
    for (i = 0; i <= xgrid; i++)
      {
      x = (int)(-360 * sin(i * segx) * sin(j * segy / 2) - 16 * sin(i * segx));
      y = (int)(-360 * cos(j * segy / 2));
      z = (int)(360 * cos(i * segx) * sin(j * segy / 2) + 16 * cos(i * segx));

      fprintf(f,"%4d, %4d, %4d, ",x,y,z);
  
      if ((++c % 4) == 0)
        fprintf(f,"\n");
      }

  fprintf(f,"\n};");

  fclose(f);

  // **** make torus mesh ****
  f = fopen("torus.h","w");

  fprintf(f,"short torus[%d] = {\n",n);

  c = 0;
  for(j = 0; j <= ygrid; j++)
    for (i = 0; i <= xgrid; i++)
      {
      x = (int)(-360 * sin(i * segx) + 120 * cos(j * segy) * sin (i * segx));
      y = (int)(-120 * sin(j * segy));
      z = (int)(360 * cos(i * segx) - 120 * cos(j * segy) * cos(i * segx));

      fprintf(f,"%4d, %4d, %4d, ",x,y,z);
  
      if ((++c % 4) == 0)
        fprintf(f,"\n");
      }

  fprintf(f,"\n};");

  fclose(f);
  }
