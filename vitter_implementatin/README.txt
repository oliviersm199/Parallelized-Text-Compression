Here is the instructions for how to run this executable:

compile to some name. Suppose we compile to a.out. Compilation is very simple, just do:

gcc vitter.c -o <executable_name>


To run the executable, just do the following:

./a.out c <file_to_encode> <random_file_name>


The original code actually writes it out on disk. I do a simple system command to clean that up but you still need to put in the file name (Easier than me having to figure out where the filename is used. The letter c is a switch that stands for encoding. 


