How to compile executables:

Use make to run the makefile in this directory. You must have Openmp installed on your computer. You will get the following executables:

- vitter: Benchmark JS Vitter program 
- freqParallel: implementation where parallelized creation of frequency table
- strategy: implementation where we parallelized testing different symbol sizes using 1,2 and 4 threads
- zipCompress: testing zip system command to compare with huffman encoding. 
- sequential: base for testing if program is executing correctly. 

To run the various executables, please give the following inputs:

./vitter c <file_to_encode> <output_file_name>
./zipCompress <file_to_encode>
./freqParallel <file_to_encode>
./strategy <file_to_encode> <num_threads> // please 1, 2 or 4
./sequential <file_to_encode> 
