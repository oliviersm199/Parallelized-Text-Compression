#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "huffman.h"

// Sequential Implementation of Huffman Encoding sourced from Rosetta Code

#define BYTES 65336

int bytes_encode(char* text_name)
{
  clock_t begin = clock();
  //loading the file into memory
  char *text = load_file(text_name);
  int num_bits = 2;

  huffcode_t **r, **e;
  int i,j;
  char strbit[MAXBITSPERCODE];
  const char *p;
 
  long bitword_freqs[BYTES];
  memset(bitword_freqs, 0, sizeof bitword_freqs);

  p = text;
  int n = strlen(p);
  char c;
  while(*p != '\0') {
	
	int bitword = 0x0000;

	for(i = 0; i < num_bits; i++){
		c = *p;
		for (j = 0; j < 8; j++) {
			bitword <<= 1;
			bitword |= !!((c << j) & 0x80);
		}

		*p++;
    }
	
	bitword_freqs[bitword]++;    
	
  }

  //free memory for text since we already got the frequency and no longer require it.
  free(text);

  e = create_huffman_codes(bitword_freqs, BYTES);
  long int new_storage = 0;

  for(i=0; i < BYTES; i++){
    if(e[i] != NULL){
      inttobits(e[i]->code, e[i]->nbits, strbit);
      new_storage += bitword_freqs[i]*strlen(strbit);
    }
  }
  clock_t end = clock();
  free_huffman_codes(e, BYTES);
  
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("\nBYTES >> Runtime     : %f seconds\n", time_spent);
  printf("BYTES >> Text # chars: %d\n",n);
  printf("BYTES >> Storage used: %d bits\n",n*8);
  printf("BYTES >> New storage : %li bits\n", new_storage);
  printf("BYTES >> Ratio       : %li %%\n", (100 - (100* new_storage/(n*8))));
  return 0;
}
                                                                                                                    
