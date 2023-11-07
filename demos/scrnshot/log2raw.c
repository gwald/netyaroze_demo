/***************************************************************
*                                                              *
*       Copyright (C) 1997 by Sony Computer Entertainment      *
*                       All rights Reserved                    *
*                                                              *
*                      S. Ashley 16th Jan 97                   *
*                                                              *
***************************************************************/

#include <stdio.h>
#include <string.h>

void main(int argc, char *argv[])
{
FILE *file_src;
FILE *file_dst;
char file_srt[17];
char file_end[18];
int srt = 0, end = 0;
unsigned char c;
unsigned long int i;
unsigned long int n;

strncpy(file_srt,"screen dump start",17);
strncpy(file_end,"screen dump finish",18);

if (argc != 3)
  printf("Usage: log2raw <source> <destination>\n");
else
  {
  file_src = fopen(argv[1],"r");
  file_dst = fopen(argv[2],"wb");
  do
    {
    c = getc(file_src);
    if (c == file_srt[srt])
      srt++;
    else
      srt = 0;
    }
  while(srt < 17 && c != EOF);

  if (c == EOF)
    {
    printf("Failed to find start of screen dump\n");
    exit(0);
    }

  do
    {
    c = getc(file_src);
    if (c == file_end[end])
      end++;
    else
      end = 0;

    if (c < 64)
      putc((c-32) << 3,file_dst);
    else if (c == 'A')
      {
      c = getc(file_src);
      putc((c-32) << 3,file_dst);
      putc((c-32) << 3,file_dst);
      }
    else if (c == 'B')
      {
      c = getc(file_src);
      putc((c-32) << 3,file_dst);
      putc((c-32) << 3,file_dst);
      putc((c-32) << 3,file_dst);
      }
    else if (c == 'C')
      {
      c = getc(file_src);
      putc((c-32) << 3,file_dst);
      putc((c-32) << 3,file_dst);
      putc((c-32) << 3,file_dst);
      putc((c-32) << 3,file_dst);
      }
    else if (c == 'D')
      {
      n = getc(file_src) - 32;
      n = n * 64 + getc(file_src) - 32;
      n = n * 64 + getc(file_src) - 32;
      n = n * 64 + getc(file_src) - 32;

      c = getc(file_src);

      for (i = 0; i <= n; i++)
        putc((c-32) << 3,file_dst);
      }

    }
  while(end < 18 && c != EOF);

  if (c == EOF)
    {
    printf("Failed to find end of screen dump\n");
    }
  }
  fclose(file_src);
  fclose(file_dst);
}