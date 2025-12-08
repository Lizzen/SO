#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
	int opt;
	int nFlag = 0; int eFlag = 0;	// Flags del opt
	int nBytes = 0;
	char buffer[BUFFER_SIZE];
	int bytes_read;
	int fd;

	// n:e porque se pueden concatenar opciones
	while ((opt = getopt(argc, argv, "n:e")) != -1){
		switch (opt){
			case 'n':
				nFlag = 1; 
				nBytes = atoi(optarg);
				break;
			case 'e':
				eFlag = 1;
				break;
			default:
				fprintf(stderr, "Usage: %s [-n num_bytes] [-e] [file]\n", argv[0]);
				exit(EXIT_FAILURE);
				break;
		}
	}

	// Verifica si no se ha especificado un fichero
	if (optind >= argc) {
		fprintf(stderr, "Expected file after options.\n");
		exit(EXIT_FAILURE);
	}

	// Abrir el fichero para lectura
	if ((fd = open(argv[optind], O_RDONLY)) < 0){
		fprintf(stderr, "Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	// Calcular el tamaño del fichero
	off_t fileSize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	if (nBytes > fileSize){
		fprintf(stderr, "The offset can't be bigger than the file size.\n");
		exit(EXIT_FAILURE);
	}
	if (nFlag){
		if (eFlag){
			// Situar el marcador de posición en el desplazamiento especificado
			lseek(fd, fileSize - nBytes, SEEK_SET);
		}
		else{
			lseek(fd, nBytes, SEEK_SET);
		}
	}

	// Mostrar el contenido del fichero
	while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0){
		write(STDOUT_FILENO, buffer, bytes_read);
	}

	if (bytes_read < 0){
		perror("Error while reading the file.");
		exit(EXIT_FAILURE);
	}
	
	close(fd);
	return 0;
}
