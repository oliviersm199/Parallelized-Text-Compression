#define MAXBITSPERCODE 100
#ifndef HUFFMAN_H_   /* Include guard */
#define HUFFMAN_H_

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

static heap_t *_heap_create(int s, long *f);
static void _heap_destroy(heap_t *heap);
static void _heap_sort(heap_t *heap);
static void _heap_add(heap_t *heap, int c);
static int _heap_remove(heap_t *heap);
huffcode_t **create_huffman_codes(long *freqs, int BYTES);
void free_huffman_codes(huffcode_t **c, int BYTES);
void inttobits(int c, int n, char *s);
char *load_file(char * path);
void writeCompressedFile(char * targetString, char *fileName, huffcode_t ***dictionary,int size, int BYTES);
void uncompressFile(char * fileName, int BYTES);


#endif // HUFFMAN_H_
