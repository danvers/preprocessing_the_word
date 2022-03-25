/* browse.c -- Byte Pair Encoding file browser */
/* Copyright 1996 Philip Gage */

#include <stdio.h>
#define LINESPERPAGE 23

unsigned char pairtable[128][2];

void decompress_page (FILE *in)
{
  unsigned char stack[16];
  short c, top=0, linecount=0;

  for (;;) {

    /* Pop byte from stack or read byte from file */
    if (top)
      c = stack[--top];
    else if ((c=getc(in)) == EOF)
      break;

    /* Push pair on stack or print byte */
    if (c & 0x80) {
      stack[top++] = pairtable[c & 0x7F][1];
      stack[top++] = pairtable[c & 0x7F][0];
    }
    else {
      putchar(c);

      /* Count linefeeds and quit after one screen */
      if (c == '\n' && ++linecount == LINESPERPAGE)
        break;
    }
  }
}

int main (int argc, char **argv)
{
  FILE *in;
  long start, end;
  int c, percent = 0;

  if (argc != 2)
    printf("Usage: browse inputfile\n");
  else if ((in=fopen(argv[1],"rb"))==NULL)
    printf("Error opening input %s\n",argv[1]);
  else {
 
    /* Check for optional pair count and pair table */
    if ((c = getc(in)) & 0x80)
      fread(pairtable,2,c & 0x7F,in);
    else
      ungetc(c,in);

    /* Find first and last byte of packed data */
    start = ftell(in);
    fseek(in,0L,SEEK_END);
    end = ftell(in);
    fseek(in,start,SEEK_SET);

    for (;;) {

      /* Display current page and read new percent */
      decompress_page(in);
      printf("\n%d %% into file, enter new %% ",percent);
      scanf("%d",&percent);

      /* Quit program if input percent out of range */
      if (percent < 0 || percent > 99)
        break;

      /* Move file pointer to new position in file */
      fseek(in,start+percent*(end-start)/100,SEEK_SET);
    }
    fclose(in);
  }
  return 0;
}