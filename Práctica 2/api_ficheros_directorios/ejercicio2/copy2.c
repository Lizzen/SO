#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFSIZE 512
void copy(int fdo, int fdd)
{
	int n_read; // n palabra que va a leer
	char buffer[BUFSIZE]; // Buffer de tamaño BUFSIZE para que vaya leyendo de 512 en 512 bytes

	// Bucle que escribe en cada itineración 512 bytes
	while ((n_read = read(fdo, buffer, BUFSIZE)) > 0){
		if (write(fdd, buffer, n_read) < n_read){
			perror("write");
			close(fdo);
			close(fdd);
			exit(EXIT_FAILURE);
		}
	}

	if (n_read < 0){
		perror("read");
		close(fdo);
		close(fdd);
		exit(EXIT_FAILURE);
	}

	close(fdo);
	close(fdd);
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
	struct stat st;

	if (lstat(orig, &st) == -1){
		perror("Error al obtener el tipo de fichero del origen");
		exit(EXIT_FAILURE);
	}


	int size = st.st_size + 1;
	char *path = malloc(size);

	ssize_t salida = readlink(orig, path, size);
	if (salida < 0){
		perror("readlink");
		exit(EXIT_FAILURE);
	}

	if (symlink(path, dest) < 0){
		perror("symlink");
		exit(EXIT_FAILURE);
	}

	free(path);
}

int main(int argc, char *argv[])
{
	if (argc < 3){
		printf("Usage: <input_file> <output_file>");
		exit(EXIT_FAILURE);	
	}

	// Comprobar el tipo de archivo origen
	struct stat st;
	
	if (lstat(argv[1], &st) == -1){
		perror("Error al obtener el tipo de fichero del origen");
		exit(EXIT_FAILURE);
	}

	// Es un archivo regular
	if (S_ISREG(st.st_mode)){
		copy_regular(argv[1], argv[2]);
	}
	// Es un enlace simbolico
	else if (S_ISLNK(st.st_mode)) {
		copy_link(argv[1], argv[2]);
	}
	// Error, no es ninguno de los dos tipos de ficheros
	else{
		printf("El fichero no es regular ni simbolico");
		exit(EXIT_FAILURE);
	}

	
	return 0;
}
