#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>

#include "huffman_word_encoding.h"
#include "huffman_encoding.h"
#include "huffman_bytes_encoding.h"
#include "huffman_2bytes_encoding.h"
#include "huffman.h"

int main(int argc, char* argv[])
{
  //validating that we have the correct number of arguments
  if(argc < 1){
    printf("Please provide a filepath to a file that may be huffman encoded as well as a name for the compressed file.\n");
    exit(-1);
  }

  //validating that the file exists
  if(access(argv[1],F_OK) == - 1){
    printf("Please provide an existing file to huffman encode\n");
    exit(-2);
  }

  char * text = load_file(argv[1]);

  long int resultRegular = regular_encode(argv[1],text);
  long int resultBytes = bytes_encode(argv[1],text);
  long int resultBits = bits_encode(argv[1],text);
  long int resultWords = word_encode(argv[1],text);
  printf("8 Bits: %li\n",resultRegular);
  printf("16 Bits: %li\n",resultBytes);
  printf("4 Bits: %li\n",resultBits);
  printf("Words: %li\n",resultWords);
}
