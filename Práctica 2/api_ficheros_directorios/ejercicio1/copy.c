#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFSIZE 512

void copy(int fdo, int fdd)
{
	int n_read;
	char buffer[BUFSIZE];
	while ((n_read = read(fdo, buffer, BUFSIZE)) > 0) {
	/* Transfer data from the buffer onto the output file */
		if (write(fdd, buffer, n_read) < n_read) {
			perror("write");
			close(fdo); 
			close(fdd);
			exit(EXIT_FAILURE);
		}
	}
	if (n_read < 0) {
		perror("read");
		close(fdo); 
		close(fdd);
		exit(EXIT_FAILURE);
	}
	close(fdo); 
	close(fdd);
}

int main(int argc, char *argv[])
{
	if (argc != 3){
		printf("Usage: %s <sinput_file> <output_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	char *inFile = argv[1];
	char *outFile = argv[2];

	int fdo = open(inFile, O_RDONLY);
	if (fdo < 0){
		perror("open fdo");
		exit(EXIT_FAILURE);
	}

	int fdd = open(outFile, O_CREAT  | O_WRONLY | O_TRUNC, 0660);
		if (fdd < 0){
		perror("open fdd");
		exit(EXIT_FAILURE);
	}

	copy (fdo, fdd);

	return 0;
}
