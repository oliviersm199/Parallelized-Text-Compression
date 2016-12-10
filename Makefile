all: freqParallel sequential strategy zip vitter

freqParallel: huffman_encoding_parallel.c
	gcc -fopenmp $^ -o $@

sequential: strategy_seq.c
	gcc huffman.c $^ huffman_2bytes_encoding.c huffman_bytes_encoding.c huffman_encoding.c huffman_word_encoding.c -fopenmp -o $@ -lm


strategy: strategy_parallelization.c
	gcc huffman.c $^ huffman_2bytes_encoding.c huffman_bytes_encoding.c huffman_encoding.c huffman_word_encoding.c -fopenmp -o $@ -lm

zip: zip.c
	gcc zip.c -o zipCompress

vitter: vitter_implementation/vitter.c
	gcc vitter_implementation/vitter.c -o vitter

