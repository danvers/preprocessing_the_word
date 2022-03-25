/* expand.c */

#include <stdio.h>

/* Decompress data from input to output */
void expand (FILE *input, FILE *output)
{
  unsigned char left[256], right[256], stack[30];
  short int c, count, i, size;

  /* Unpack each block until end of file */
  while ((count = getc(input)) != EOF) {

    /* Set left to itself as literal flag */
    for (i = 0; i < 256; i++)
      left[i] = i;

    /* Read pair table */
    for (c = 0;;) {

      /* Skip range of literal bytes */
      if (count > 127) {
        c += count - 127;
        count = 0;
      }
      if (c == 256) break;

      /* Read pairs, skip right if literal */
      for (i = 0; i <= count; i++, c++) {
        left[c] = getc(input);
        if (c != left[c])
          right[c] = getc(input);
      }
      if (c == 256) break;
      count = getc(input);
    }

    /* Calculate packed data block size */
    size = 256 * getc(input);
    size += getc(input);

    /* Unpack data block */
    for (i = 0;;) {

      /* Pop byte from stack or read byte */
      if (i)
        c = stack[--i];     
      else {
        if (!size--) break;
        c = getc(input);
      }

      /* Output byte or push pair on stack */
      if (c == left[c])      
        putc(c,output);
      else {
        stack[i++] = right[c];
        stack[i++] = left[c];
      }
    }
  }
}

void main (int argc, char *argv[])
{
  FILE *infile, *outfile;

  if (argc != 3)
    printf("Usage: expand infile outfile\n");
  else if ((infile=fopen(argv[1],"rb"))==NULL)
    printf("Error opening input %s\n",argv[1]);
  else if ((outfile=fopen(argv[2],"wb"))==NULL)
    printf("Error opening output %s\n",argv[2]);
  else {
    expand(infile,outfile);
    fclose(outfile);
    fclose(infile);
  }
}
