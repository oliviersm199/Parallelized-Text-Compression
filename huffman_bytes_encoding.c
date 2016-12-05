#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "huffman.h"

// Sequential Implementation of Huffman Encoding sourced from Rosetta Code

#define BYTES 16

int bits_encode(char* text_name)
{
  clock_t begin = clock();
  //loading the file into memory
  char *text = load_file(text_name);

  huffcode_t **r, **e;
  int i;
  char strbit[MAXBITSPERCODE];
  const char *p;
  int byte_length = 4;
  int bitword_size = (int) pow(2.0, (double) byte_length);

  long freqs[BYTES];
  long bitword_freqs[bitword_size];
  memset(freqs, 0, sizeof freqs);
  memset(bitword_freqs, 0, sizeof bitword_freqs);

  p = text;
  int n = strlen(p);

  while(*p != '\0') {
	char c = *p;

	//for (i = 0; i < 8; i++) {
        //        printf("%d", !!((c << i) & 0x80));
        //}
        //printf("%s", "\n");

	
	int mask_1 = 0x0f;
	int mask_2 = 0xf0;

	int first = c & mask_1;
	//printf("%d\n", first);

	int second = c & mask_2;
	second = second >> 4;
	//printf("%d\n", second);
	
	bitword_freqs[first]++;
	bitword_freqs[second]++;

	*p++;

	//freqs[*p++]++;
  }


  e = create_huffman_codes(bitword_freqs, BYTES);
  long int new_storage = 0;

  for(i=0; i < BYTES; i++){
    if(e[i] != NULL){
      inttobits(e[i]->code, e[i]->nbits, strbit);
      new_storage += bitword_freqs[i]*strlen(strbit);
      //printf("4Bits: %d,(%d),%s,%li\n", i, e[i]->code,strbit,bitword_freqs[i]);
    }
  }
  
  long int overhead = getOverhead(&e,BYTES,4);


  free(text);
  free_huffman_codes(e, BYTES);

  clock_t end = clock();

  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  
  printf("\n4 Bits >> Runtime     : %f seconds\n", time_spent);
  printf("4 Bits >> Text # chars: %d\n",n);
  printf("4 Bits >> Old: %d bits\n",n*8);
  printf("4 Bits >> New: %li bits\n", new_storage);
  printf("4 Bits >> Overhead: %li bits \n",overhead);
  printf("4 Bits >> Total: %li bits \n",new_storage + overhead);
  printf("4 Bits >> Ratio       : %li %%\n", (100 - (100* (new_storage+overhead)/(n*8))));

  return 0;
}
                                                                                                                    
