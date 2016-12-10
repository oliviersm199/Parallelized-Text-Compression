#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>


#include "huffman.h"
#include "huffman_word_encoding.h"
#include "huffman_encoding.h"
#include "huffman_bytes_encoding.h"
#include "huffman_2bytes_encoding.h"


#define NUM_STRATEGY 4

int main(int argc, char* argv[])
{
  //validating that we have the correct number of arguments
  if(argc < 3){
    printf("Please provide a filepath to a file that may be huffman encoded and the number of threads to use\n");
    exit(-1);
  }

  //validating that the file exists
  if(access(argv[1],F_OK) == - 1){
    printf("Please provide an existing file to huffman encode\n");
    exit(-2);
  }


  // loading 
  char * text = load_file(argv[1]);
  int numThreads = atoi(argv[2]);

	#pragma omp parallel num_threads(numThreads) 
  	{
		int thread_id = omp_get_thread_num();
		int workEach = NUM_STRATEGY/omp_get_num_threads(); 
		int startPoint = workEach * thread_id;
		long int localResult;
		char * strategyName;
		for(int i = startPoint; i<startPoint + workEach;i++){
			//printf("Thread Id:%d, running algorithm %d\n",thread_id,i);
			switch(i){
				case 0:
					localResult = regular_encode(argv[1],text);
					strategyName = "8 Bit";
					break;
				case 1:
					localResult = bytes_encode(argv[1],text);
					strategyName = "16 Bit";
					break;
				case 2:
					localResult =  bits_encode(argv[1],text);
					strategyName = "4 Bit";
					break;
				case 3:
					localResult =  word_encode(argv[1],text);
					strategyName = "Words";
					break; 
			}
			printf("Result: %s,  %li\n",strategyName,localResult);
		}
    }
    free(text);
}
