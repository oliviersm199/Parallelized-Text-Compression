#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

// Sequential Implementation of Huffman Encoding sourced from Rosetta Code

#define BYTES 16

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

#define swap_(I,J) do { int t_; t_ = a[(I)];    \
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

  for(i=0; i < BYTES; i++) {
    if ( efreqs[i] > 0 ) _heap_add(heap, i);
  }

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

char * load_file(char * path){

  FILE *fp;
  fp = fopen(path,"rb");
  unsigned long length;

  char*buffer = 0;

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
  buffer[length] = '\0';
  return buffer;
}


int main(int argc,char* argv[])
{
  //validating that we have the correct number of arguments
  if(argc < 2){
    printf("Please provide a filepath to a file that may be huffman encoded.\n");
    exit(-1);
  }

  //validating that the file exists
  if( access(argv[1],F_OK) == - 1){
    printf("Please provide an existing file to huffman encode\n");
    exit(-2);
  }

  //loading the file into memory
  char *text = load_file(argv[1]);

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

  //free memory for text since we already got the frequency and no longer require it.
  free(text);

  e = create_huffman_codes(bitword_freqs);
  long int new_storage = 0;

  printf("Bit number    | Frequency\n");
  
  for(i=0; i < BYTES; i++){
    printf("%d: %li\n", i, bitword_freqs[i]);
  }

  printf("\nBit number 	(Value of encoding) Encoding\n");
  for(i=0; i < BYTES; i++){
    if(e[i] != NULL){
      inttobits(e[i]->code, e[i]->nbits, strbit);
      new_storage += bitword_freqs[i]*strlen(strbit);
      printf("%d 	(%d) %s\n", i, e[i]->code, strbit);
    }
  }
  free_huffman_codes(e);
  printf("\n\n");
  printf("Text # chars: %d\n",n);
  printf("Storage used: %d bits\n",n*8);

  printf("New storage : %li bits\n", new_storage);
  return 0;
}
                                                                                                                    


/*

int main(){
	char word [] = "Hello World. My name is Eduardo";
	int n = strlen(word);
	int i, j;
	
	for (j = 0; j < n; j++){
		char c = (char) word[j];
		printf("%c: ",c);
		for (i = 0; i < 8; i++) {
      			printf("%d", !!((c << i) & 0x80));
		}
		printf("%s", "\n");
	}
	return 0;
}
*/
