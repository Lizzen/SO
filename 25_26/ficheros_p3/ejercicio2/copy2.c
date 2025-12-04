#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


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

void copy_regular(char *orig, char *dest)
{
	int fdi = open(orig, O_RDONLY);
	if (fdi < 0){
		perror("Error al abrir el input file");
		exit(EXIT_FAILURE);
	}

	int fdo = open(dest, O_CREAT | O_WRONLY | O_TRUNC, 0660);
	if (fdo < 0){
		perror("Error al abrir o crear el output file");
		exit(EXIT_FAILURE);
	}

	copy(fdi, fdo);
}

void copy_link(char *orig, char *dest)
{
	struct stat sb;
	char *buffer;
	ssize_t n, bufSize;

	if (lstat(orig, &sb) < 0){
		perror("lstat\n");
		exit(EXIT_FAILURE);
	}

	bufSize = sb.st_size + 1;
	buffer = malloc(bufSize);
	n = readlink(dest, buffer, bufSize);

	symlink(buffer, dest);

	free(buffer);
	
}

int main(int argc, char *argv[])
{
	if (argc != 3){
		fprintf(stderr, "Usage: %s <source file> <destination file>\n", argv[0]);
		return -1;
	}

	int fdin = open(argv[1], O_RDONLY);
	if (fdin == -1){
		perror("Error opening source file\n;");
		return -1;
	}

	int fdout = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fdin == -1){
		perror("Error opening source file\n;");
		return -1;
	}



	return 0;
}
