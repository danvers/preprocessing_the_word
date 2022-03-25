/* compress.c -- Byte Pair Encoding compression */
/* Copyright 1996 Philip Gage */

#include <stdio.h>
#include <stdlib.h>
/* Added by T. Shoemaker */
#include <string.h>

#define MAXSIZE 65535L  /* Input file buffer size */
#define HASHSIZE 8192   /* Hash table size, power of 2 */
#define THRESHOLD   3   /* Increase for speed, min 3 */

void compress (FILE *input, FILE *output)
{
  unsigned char *buffer, *left, *right, *count;
  unsigned char a, b, bestcount=0, pairtable[128][2];
  int i, j, index, bestindex, code=128;
  size_t size;

  /* Dynamically allocate buffers and check for errors */
  buffer = (unsigned char *)malloc(MAXSIZE);
  left = (unsigned char *)malloc(HASHSIZE);
  right = (unsigned char *)malloc(HASHSIZE);
  count = (unsigned char *)malloc(HASHSIZE);
  if (buffer==NULL || left==NULL ||
      right==NULL || count==NULL) {
    printf("Error allocating memory\n");
    exit(1);
  }

  /* Read input file into buffer and check for errors */
  size = fread(buffer,1,MAXSIZE,input);
  if (size == MAXSIZE) {
    printf("File too big\n");
    exit(1);
  }
  for (i=0; i<size; i++)
    /*
    * The `if` statement below checks to see whether all the
    * plaintext characters in the buffer are under ASCII 127 
    * (i.e. they are single-byte characters). All characters 
    * from ASCII 128-256 are reserved for encoding the data 
    * in the buffer. The result: anything in the expanded 
    * ASCII set, and by extension, Unicode now, is not 
    * considered to be text. WordPiece will reverse this logic
    * almost entirely: the whole point of subword tokenizers 
    * is to use the byte-pair logic to a) expand a vocabulary
    * and b) enable multi-scriptural text modeling.
    *
    * We'll add a simple print statement in the check to see
    * which codes throw an error.
    *                                                   - T.S.
    */
    if (buffer[i] > 127) {
      /* Added next line (but not the one after!) - T.S. */
      printf("Break: %04X\n", buffer[i]);
      printf("This program works only on text files\n");
      exit(1);
    }

  do {  /* Replace frequent pairs with bytes 128..255 */

    /* Enter counts of all byte pairs into hash table */
    memset(count,0,HASHSIZE);
    for (i=0; i<size-1; i++) {
      a = buffer[i];
      b = buffer[i+1];
      index = (a ^ (b << 6)) & (HASHSIZE-1);
      while ((left[index] != a || right[index] != b) &&
             count[index] != 0)
        index = (index + 1) & (HASHSIZE-1);
      left[index] = a;
      right[index] = b;
      if (count[index] < 255)
        ++count[index];
    }

    /* Search hash table for most frequent pair */
    bestcount = THRESHOLD - 1;
    for (i=0; i<HASHSIZE; i++) {
      if (count[i] > bestcount) {
        bestcount = count[i];
        bestindex = i;
      }
    }

    /* Compress if enough occurrences of pair */
    if (bestcount >= THRESHOLD) {

      /* Add pair to table using code as index */
      a = pairtable[code-128][0] = left[bestindex];
      b = pairtable[code-128][1] = right[bestindex];

      /* Replace all pair occurrences with unused byte */
      for (i=0, j=0; i<size; i++, j++)
        if (a == buffer[i] && b == buffer[i+1]) {
          buffer[j] = code;
          ++i;
        }
        else
          buffer[j] = buffer[i];
      size = j;
    }
    else
      break;
  } while (++code < 255);

  /* Write pair count, pair table and compressed data */
  putc(code,output);
  fwrite(pairtable,2,code-128,output);
  fwrite(buffer,1,size,output);
  free(buffer); free(left); free(right); free(count);
}

int main (int argc, char **argv)
{
  FILE *in, *out;

  if (argc != 3)
    printf("Usage: compress inputfile outputfile\n");
  else if ((in=fopen(argv[1],"rb"))==NULL)
    printf("Error opening input %s\n",argv[1]);
  else if ((out=fopen(argv[2],"wb"))==NULL)
    printf("Error opening output %s\n",argv[2]);
  else {
    compress(in,out);
    fclose(out);
    fclose(in);
  }
  return 0;
}
