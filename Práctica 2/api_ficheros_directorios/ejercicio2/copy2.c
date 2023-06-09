#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

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

void copy_regular(char *orig, char *dest)
{
	int fdo = open(orig, O_RDONLY);
	if (fdo < 0){
		perror("open fdo");
		exit(EXIT_FAILURE);
	}

	int fdd = open(dest, O_CREAT  | O_WRONLY | O_TRUNC, 0660);
		if (fdd < 0){
		perror("open fdd");
		exit(EXIT_FAILURE);
	}

	copy (fdo, fdd);
}

void copy_link(char *orig, char *dest)
{
	struct stat st;

	//Obtenerinfo del enlace simbóñico
	if (lstat(orig, &st) == -1){
		perror("Error al intentar obtener información sobre el enlace simbólico");
		exit(EXIT_FAILURE);
	}

	// Comprobar si el archivo origen es un enlace simbólico
	if (!S_ISLNK(st.st_mode)){
		perror("El archivo no es un enlace simbólico");
		exit(EXIT_FAILURE);
	}

	// Reservar memoria para la ruta del archivo apuntado
	size = st.st_size +1;
	char *path = malloc(size);
	if (path == NULL) {
		perror("Error malloc");
		exit(EXIT_FAILURE);
	}

    // Leer la ruta del archivo apuntado por el enlace simbólico
    ssize_t result = readlink(orig, path, size);
    if (result == -1) {
        perror("Error readlink");
        exit(EXIT_FAILURE);
    }
    target_path[result] = '\0'; // Añadir manualmente el carácter null de final de cadena

    // Crear un nuevo enlace simbólico que apunte a la misma ruta
    if (symlink(path, dest) == -1) {
        perror("Error symlink");
        exit(EXIT_FAILURE);
    }

    free(path);
}

int main(int argc, char *argv[])
{
	if (argc != 3){
		printf("Usage: %s <sinput_file> <output_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

    // Comprobar el tipo de archivo origen y llamar a la función correspondiente
    struct stat st;
    if (lstat(argv[1], &st) == -1) {
        perror("Error al obtener información del archivo origen");
        exit(EXIT_FAILURE);
    }

	if (S_ISREG(st.st_mode)) {
        copy_regular(argv[1], argv[2]);
    } else if (S_ISLNK(st.st_mode)) {
        copy_link(argv[1], argv[2]);
    } else {
        fprintf(stderr, "El archivo origen no es ni un archivo regular ni un enlace simbólico\n");
        exit(1);
    }

	return 0;
}
