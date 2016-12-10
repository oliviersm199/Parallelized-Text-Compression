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

 // #pragma omp parallel num_threads(1)
//  {
//	int thread_id = omp_get_thread_num();
    //printf("%d", thread_id);
//	switch(thread_id){
//		case 0:
			regular_encode(argv[1]);
//			break;
//		case 1:
			bytes_encode(argv[1]);
//			break;
//		case 2:
			bits_encode(argv[1]);
//			break;
//		case 3:
			word_encode(argv[1]);
//			break; 
//	}
 // }
}
