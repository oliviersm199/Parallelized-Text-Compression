#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <time.h>
#include "huffman.h"
#include "huffman_encoding.h"

// Sequential Implementation of Huffman Encoding sourced from Rosetta Code

int regular_encode(char* text_name)
{
  clock_t begin = clock();
  int BYTES = 256;
  //loading the file from memory into p and getting the size
  char a;
  char *p;
  huffcode_t **r;
  int i;
  char strbit[MAXBITSPERCODE];

  char *text = load_file(text_name);
  p = text;
  int n = strlen(p);
  //get the frequency of characters in the text
  long freqs[BYTES];

  memset(freqs, 0, sizeof freqs);
  while(*p != '\0') freqs[*p++]++;

  r = create_huffman_codes(freqs, BYTES);

  //writeCompressedFile(p,output_file,&r,textLength, BYTES);

  //uncompressFile(output_file, BYTES);
  //from here the file should exist. Lets

  long int new_storage = 0;
  for(i=0; i < BYTES; i++){
    if(r[i] != NULL){
      inttobits(r[i]->code, r[i]->nbits, strbit);
      new_storage += freqs[i]*strlen(strbit);
      //printf("%d 	(%d) %s\n", i, e[i]->code, strbit);
    }
  }
  

  long int overhead = getOverhead(&r,BYTES,8);
  
  free_huffman_codes(r, BYTES);

  


  clock_t end = clock();

  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  
  printf("\n8 Bits >> Runtime     : %f seconds\n", time_spent);
  printf("8 Bits >> Text # chars: %d\n",n);
  printf("8 Bits >> Old: %d bits\n",n*8);
  printf("8 Bits >> New: %li bits\n", new_storage);
  printf("8 Bits >> Overhead: %li bits \n",overhead);
  printf("8 Bits >> Total: %li bits \n",new_storage + overhead);
  printf("8 Bits >> Ratio       : %li %%\n", (100 - (100* (new_storage+overhead)/(n*8))));
  return 0;
}
