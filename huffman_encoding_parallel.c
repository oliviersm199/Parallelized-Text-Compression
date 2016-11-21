#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <omp.h>
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
  clock_t begin = clock();

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
  
  huffcode_t **r; 
  int i;
  char strbit[MAXBITSPERCODE];
  const char *p;
  long freqs[BYTES];
  memset(freqs, 0, sizeof freqs);
 
  p = text;
  
  int size = 0;
  int idx;
  while(*p != '\0') {
    *p++; 
    size++;
  }

  printf("size is %d\n\n", size);

  #pragma omp parallel num_threads(32)
  {
    int tid = omp_get_thread_num();
    int chunk_size = size / omp_get_num_threads();
    int start_idx = tid * chunk_size;
    int end_idx = (tid == omp_get_num_threads()-1) ? size : start_idx + chunk_size;
    int idx;
    long myfreqs[BYTES] = {0};
    char *temp = &text[start_idx];
    for (idx = start_idx; idx < end_idx; idx++) {
      myfreqs[*temp++]++;
    }
    int i;
    for (i =0; i < BYTES; i++) {
      #pragma omp atomic
      freqs[i] += myfreqs[i];
    }
  }
  
 /* while(*p != '\0') {
    printf("Found %c\n", *p);
    freqs[*p++]++;
  } */

  //free memory for text since we already got the frequency and no longer require it.
  free(text);

 
  r = create_huffman_codes(freqs);
 
  for(i=0; i < BYTES; i++) {
    if ( r[i] != NULL ) {
      inttobits(r[i]->code, r[i]->nbits, strbit);
      printf("%c (%d) %s\n", i, r[i]->code, strbit);
    }
  }


 
  free_huffman_codes(r);

  clock_t end = clock();

  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Runtime: %f seconds\n", time_spent);

  return 0;
}
