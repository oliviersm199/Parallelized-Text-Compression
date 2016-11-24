#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <time.h>

// Sequential Implementation of Huffman Encoding sourced from Rosetta Code

#define BYTES 256

struct huffcode {
  int nbits;
  int code;
};
typedef struct huffcode huffcode_t;

struct huffheap {
  int *h;
  int n, s, cs;
  long *f;
};
typedef struct huffheap heap_t;

/* heap handling funcs */
static heap_t *_heap_create(int s, long *f)
{
  heap_t *h;
  h = malloc(sizeof(heap_t));
  h->h = malloc(sizeof(int)*s);
  h->s = h->cs = s;
  h->n = 0;
  h->f = f;
  return h;
}

static void _heap_destroy(heap_t *heap)
{
  free(heap->h);
  free(heap);
}

#define swap_(I,J) do { int t_; t_ = a[(I)];	\
      a[(I)] = a[(J)]; a[(J)] = t_; } while(0)
static void _heap_sort(heap_t *heap)
{
  int i=1, j=2; /* gnome sort */
  int *a = heap->h;

  while(i < heap->n) { /* smaller values are kept at the end */
    if ( heap->f[a[i-1]] >= heap->f[a[i]] ) {
      i = j; j++;
    } else {
      swap_(i-1, i);
      i--;
      i = (i==0) ? j++ : i;
    }
  }
}
#undef swap_

static void _heap_add(heap_t *heap, int c)
{
  if ( (heap->n + 1) > heap->s ) {
    heap->h = realloc(heap->h, heap->s + heap->cs);
    heap->s += heap->cs;
  }
  heap->h[heap->n] = c;
  heap->n++;
  _heap_sort(heap);
}

static int _heap_remove(heap_t *heap)
{
  if ( heap->n > 0 ) {
    heap->n--;
    return heap->h[heap->n];
  }
  return -1;
}

/* huffmann code generator */
huffcode_t **create_huffman_codes(long *freqs)
{
  huffcode_t **codes;
  heap_t *heap;
  long efreqs[BYTES*2];
  int preds[BYTES*2];
  int i, extf=BYTES;
  int r1, r2;

  memcpy(efreqs, freqs, sizeof(long)*BYTES);
  memset(&efreqs[BYTES], 0, sizeof(long)*BYTES);

  heap = _heap_create(BYTES*2, efreqs);
  if ( heap == NULL ) return NULL;

  for(i=0; i < BYTES; i++) if ( efreqs[i] > 0 ) _heap_add(heap, i);

  while( heap->n > 1 )
  {
    r1 = _heap_remove(heap);
    r2 = _heap_remove(heap);
    efreqs[extf] = efreqs[r1] + efreqs[r2];
    _heap_add(heap, extf);
    preds[r1] = extf;
    preds[r2] = -extf;
    extf++;
  }
  r1 = _heap_remove(heap);
  preds[r1] = r1;
  _heap_destroy(heap);

  codes = malloc(sizeof(huffcode_t *)*BYTES);

  int bc, bn, ix;
  for(i=0; i < BYTES; i++) {
    bc=0; bn=0;
    if ( efreqs[i] == 0 ) { codes[i] = NULL; continue; }
    ix = i;
    while( abs(preds[ix]) != ix ) {
      bc |= ((preds[ix] >= 0) ? 1 : 0 ) << bn;
      ix = abs(preds[ix]);
      bn++;
    }
    codes[i] = malloc(sizeof(huffcode_t));
    codes[i]->nbits = bn;
    codes[i]->code = bc;
  }
  return codes;
}

void free_huffman_codes(huffcode_t **c)
{
  int i;

  for(i=0; i < BYTES; i++) free(c[i]);
  free(c);
}

#define MAXBITSPERCODE 100

void inttobits(int c, int n, char *s)
{
  s[n] = 0;
  while(n > 0) {
    s[n-1] = (c%2) + '0';
    c >>= 1; n--;
  }
}

/*
  returns length of string loaded into file and sets buffer equal to path
  takes a double pointer in return to the calling function and set the value of
  the string to the value uploaded in this method.
*/
int load_file(char * path, char ** bufferPtr){
  //deference to make easier to reference
  char *buffer = *bufferPtr;

  // opening file, checking the length and reading it into the buffer.
  FILE *fp;
  fp = fopen(path,"rb");
  int length;
  if(fp){
    fseek(fp,0,SEEK_END);
    length = ftell(fp);
    rewind(fp);
    buffer = (char *) malloc((length+1)*sizeof(char));
    if(buffer){
        fread(buffer,sizeof(char),length,fp);
    }
    fclose(fp);
  }
  //setting the last character to the terminating character
  *(buffer + length) = '\0';

  //resetting bufferPtr to point to buffer and returning the length
  *bufferPtr = buffer;
  return length;
}


/*
    This function will write a string to a file using our huffman encoding, writing byte by byte.
    If a particular translation for a character has > 32 bits, then the function will not copy
    correctly, since it uses a buffer of 32 bits to write to the file. 
*/
void writeCompressedFile(char * targetString, char *fileName, huffcode_t ***dictionary,int size){
  huffcode_t **r = *dictionary;
  FILE *fp = fopen(fileName,"wb");
  
  //get the size of our alphabet
  uint32_t alphabetSize = 0;
  for(int i=0; i < BYTES; i++) {
    if ( r[i] != NULL ) {
      alphabetSize++;  
    }
  }
  fwrite(&alphabetSize,4,1,fp); 

  //write the alphabet down in the following way:
  // alphabetnumBitsRepresentbits  where alphabet = 8 bits, numBits = 32 and bitsRepresent = 32 bits  
  for(int i=0;i<BYTES;i++){
    if(r[i] != NULL){
	fwrite(&i,1,1,fp); //writing the character to file
	fwrite(&r[i]->nbits,4,1,fp); //writing the number of bits in representation
	fwrite(&r[i]->code,4,1,fp); // writing the bits in representation
    }
  }

  // write the actual text to the file 
  uint32_t buffer = 0;
  int bufbits = 0;
  for(int i = 0; i < size;i++){
    int targetChar = targetString[i];
    int value = r[targetChar] -> code;
    int nBits = r[targetChar] -> nbits;
    buffer <<= nBits; // making room for the bits
    bufbits += nBits; // increment the number of bits in the buffer
    buffer |= value;  // saving hte value in the buffer 
    while(bufbits >=8)
    {
        bufbits -=8;
	fputc((buffer >> bufbits)&0xFF,fp);	
    }
  }
  //some left over bits, we know it's less than 8 
  if(bufbits > 0){
        char leftOver = buffer;
	leftOver<<(8-bufbits);
        fputc(leftOver,fp); 
  }

  fclose(fp);
}

//uncompress the file 
void uncompressFile(char * fileName){
	// open the file pointer
	FILE *fp = fopen(fileName,"rb");
	int errnum;
	fp = fopen(fileName,"rb");
        //validate that file was opened successfully	
	if(fp == NULL){
		errnum = errno;
		fprintf(stderr,"Error Code: %d\n",errno);
		perror("Error");
		fprintf(stderr,"Error opening file: %s\n",strerror(errnum));
		exit(errnum);
	}

	//attempt to read in first 32 bits which represent the size of the alphabet. 
	uint32_t alphabetSize = 0;
	fread(&alphabetSize,4,1,fp);
	printf("Alphabet Size Uncompressed: %d\n",alphabetSize);

	//create space in memory for alphabetSize alphabet dictionary
	huffcode_t **r = malloc(sizeof(huffcode_t *)*BYTES);
	
	char tempchar;
	int nbits;
	int symbol;
	char * strbit = 0;
	for(int i = 0; i < alphabetSize;i++){
		fread(&tempchar,1,1,fp);
		fread(&nbits,4,1,fp);
		fread(&symbol,4,1,fp);
		printf("Symbol:%c NumBits:%d Symbol:%d\n",tempchar,nbits,symbol);	
	}
	//done
	fclose(fp);
}


int main(int argc,char* argv[])
{
  clock_t begin = clock();

  //validating that we have the correct number of arguments
  if(argc < 3){
    printf("Please provide a filepath to a file that may be huffman encoded as well as a name for the compressed file.\n");
    exit(-1);
  }

  //validating that the file exists
  if( access(argv[1],F_OK) == - 1){
    printf("Please provide an existing file to huffman encode\n");
    exit(-2);
  }

  //loading the file from memory into p and getting the size
  char a;
  char *p;
  int textLength = load_file(argv[1],&p);
  huffcode_t **r;
  int i;
  char strbit[MAXBITSPERCODE];
 


  //get the frequency of characters in the text
  long freqs[BYTES];

  memset(freqs, 0, sizeof freqs);
  while(*p != '\0') freqs[*p++]++;

  //resetting p back to beginning
  p = p - textLength;

  //creating the huffman codes by generating a priority queue from the frequency of
  //the characters in the text
  r = create_huffman_codes(freqs);

  writeCompressedFile(p,argv[2],&r,textLength);

  uncompressFile(argv[2]);
  //from here the file should exist. Lets

  free_huffman_codes(r);
  clock_t end = clock();

  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Runtime: %f seconds\n", time_spent);
  return 0;
}
