#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "huffman.h"
#define BYTES 2000
 
int calculate_frequencies(char *str, long *freqs, char words[BYTES][500], int size)
{
  int count = 0, c = 0, i, j = 0, k, space = 0;
    char p[BYTES][500], ptr1[BYTES][500];
    char *ptr;
 
    for (i = 0; i < size; i++)
    {
        if ((str[i] == ' ') || (str[i] == ',') || (str[i] == '.'))
        {
            space++;
        }
    }
    for (i = 0, j = 0, k = 0;j < size; j++)
    {
        if ((str[j] == ' ') ||(str[j] == 44) || (str[j] == 46))  
        {    
            p[i][k] = '\0';
            i++;
            k = 0;
        }        
        else
             p[i][k++] = str[j];
    }
    k = 0;
    for (i = 0;i <= space;i++)
    {
        for (j = 0;j <= space;j++)
        {
            if (i == j)
            {
                strcpy(ptr1[k], p[i]);
                k++;
                count++;
                break;
            }
            else
            {
                if (strcmp(ptr1[j], p[i]) != 0)
                    continue;
                else
                    break;
            }
        }
    }
    for (i = 0;i < count;i++) 
    {
        for (j = 0;j <= space;j++)
        {
            if (strcmp(ptr1[i], p[j]) == 0)
                c++;
        }
        //printf("%s -> %d times\n", ptr1[i], c);
        strcpy(words[i],ptr1[i]);
        freqs[i] = c;
        c = 0;
    }
}

int word_encode(char* text_name,char * textInput)
{
  clock_t begin = clock();
  //loading the file into memory 
  char *text = textInput;
  
  huffcode_t **r; 
  int i;
  char strbit[MAXBITSPERCODE];
  const char *p;
  long freqs[BYTES];
  char words[BYTES][500];
  memset(freqs, 0, sizeof freqs);
 
  p = text;

  int size = 0;
  int idx;
  while(*p != '\0') {
    *p++; 
    size++;
  }

 // while(*p != '\0') freqs[*p++]++;
  calculate_frequencies(text, freqs, words, size);

 
  r = create_huffman_codes(freqs, BYTES);
  long int new_storage = 0;
  long int overhead = 0; 

  for(i=0; i < BYTES; i++) {
    if ( r[i] != NULL ) {
      //printf("%ld\n", freqs[i]);
      inttobits(r[i]->code, r[i]->nbits, strbit);
      new_storage += freqs[i]*strlen(strbit);
      //printf("Words:%s,%d,%s,%li\n", words[i], r[i]->code, strbit,freqs[i]);
      overhead += (strlen(words[i]) * 8); // need to store the actual word, character by character  
      overhead += 5; // need to store how many bits in code
      overhead += (r[i] -> nbits); // and store actual bits
    }
  }
 
  free_huffman_codes(r, BYTES);

  clock_t end = clock();

  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  //printf("\nWords >> Runtime     : %f seconds\n", time_spent);
  //printf("Words >> Text # chars: %d\n",size);
  //printf("Words >> Old: %d bits\n",size*8);
  //printf("Words >> New: %li bits\n", new_storage);
  //printf("Words >> Overhead: %li bits \n",overhead);
  //printf("Words >> Total: %li bits \n",new_storage + overhead);
  //printf("Words >> Ratio       : %li %%\n", (100 - (100* (new_storage+overhead)/(size*8))));
  
  return (new_storage + overhead);
}
