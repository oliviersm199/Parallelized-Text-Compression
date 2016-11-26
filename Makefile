all: parallel strategy

parallel: huffman_encoding_parallel.c
	gcc -fopenmp $^ -o $@

strategy: strategy_parallelization.c

	gcc huffman.c $^ huffman_2bytes_encoding.c huffman_bytes_encoding.c huffman_encoding.c huffman_word_encoding.c -fopenmp -o $@ -lm

clean:
	-rm parallel strategy *.o

