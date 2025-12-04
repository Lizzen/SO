#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>	

void copy(int fdo, int fdd)
{
	int n;
	char buffer[1024];

	while((n = read(fdo, buffer, sizeof(buffer)))> 0){
		if (write(fdd, buffer, n) == -1){
			perror("Error copying the information of the source to the destination");
			return;
		}
	}

	close(fdo); close(fdd);
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <source file> <destination file>\n", argv[0]);
		return 1;
	}
	int fdin = open(argv[1], O_RDONLY);
	if (fdin == -1) {
		perror("Error opening source file");
		return 1;
	}
	int fdout = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fdout == -1) {
		perror("Error opening destination file");
		return 1;
	}

	copy(fdin, fdout);
	return 0;
}
