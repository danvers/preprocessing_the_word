/* expand.c -- Byte Pair Encoding decompression */
/* Copyright 1996 Philip Gage */

#include <stdio.h>

void decompress (FILE *in, FILE *out)
{
  unsigned char stack[16], pair[128][2];
  short c, top = 0;

  /* Check for optional pair count and pair table */
  if ((c = getc(in)) > 127)
    fread(pair,2,c-128,in);
  else
    putc(c,out);

  for (;;) {

    /* Pop byte from stack or read byte from file */
    if (top)
      c = stack[--top];
    else if ((c = getc(in)) == EOF)
      break;

    /* Push pair on stack or output byte to file */
    if (c > 127) {
      stack[top++] = pair[c-128][1];
      stack[top++] = pair[c-128][0];
    }
    else
      putc(c,out);
  }
}

int main (int argc, char **argv)
{
  FILE *in,*out;

  if (argc != 3)
    printf("Usage: expand inputfile outputfile\n");
  else if ((in=fopen(argv[1],"rb"))==NULL)
    printf("Error opening input %s\n",argv[1]);
  else if ((out=fopen(argv[2],"wb"))==NULL)
    printf("Error opening output %s\n",argv[2]);
  else {
    decompress(in,out);
    fclose(out);
    fclose(in);
  }
  return 0;
}