#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int main(int argc, char **argv){

	char *filename = argv[1];
	clock_t start = clock();
	char command[50] = "gzip ";
	strcat(command, filename);
	system(command);
	clock_t end = clock();
	float time = (float) (end-start)/ CLOCKS_PER_SEC;
	printf("Time: %fs\n", time);

		
}
