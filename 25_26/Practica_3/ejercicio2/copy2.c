#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

/**
 * Copia el contenido de un archivo a otro usando descriptores de archivo.
 * @param fdo Descriptor del archivo origen (lectura)
 * @param fdd Descriptor del archivo destino (escritura)
 * @return 0 si la copia fue exitosa, -1 en caso de error
 */
int copy(int fdo, int fdd)
{
	int n;
	char buffer[1024];
	int result = 0;

	// Lee del archivo origen y escribe en el archivo destino
	while ((n = read(fdo, buffer, sizeof(buffer))) > 0) {
		if (write(fdd, buffer, n) == -1) {
			perror("Error writing to destination file");
			result = -1;
			break;
		}
	}

	// Verifica si hubo error en la lectura
	if (n == -1) {
		perror("Error reading from source file");
		result = -1;
	}

	// Cierra ambos archivos
	if (close(fdo) == -1) {
		perror("Error closing source file");
		result = -1;
	}
	
	if (close(fdd) == -1) {
		perror("Error closing destination file");
		result = -1;
	}

	return result;
}

/**
 * Copia un archivo regular.
 * @param orig Ruta del archivo origen
 * @param dest Ruta del archivo destino
 * @return 0 si la copia fue exitosa, -1 en caso de error
 */
int copy_regular(char *orig, char *dest)
{
	int fdi = open(orig, O_RDONLY);
	if (fdi < 0) {
		perror("Error opening source file");
		return -1;
	}

	int fdo = open(dest, O_CREAT | O_WRONLY | O_TRUNC, 0660);
	if (fdo < 0) {
		perror("Error opening or creating destination file");
		close(fdi);  // Cierra el archivo origen antes de salir
		return -1;
	}

	int result = copy(fdi, fdo);
	return result;
}

/**
 * Copia un enlace simbólico.
 * @param orig Ruta del enlace simbólico origen
 * @param dest Ruta del enlace simbólico destino
 * @return 0 si la copia fue exitosa, -1 en caso de error
 */
int copy_link(char *orig, char *dest)
{
	struct stat sb;
	char *buffer;
	ssize_t n;
	size_t bufSize;

	// Obtiene información del enlace simbólico
	if (lstat(orig, &sb) < 0) {
		perror("Error getting symlink information");
		return -1;
	}

	// Reserva memoria para el buffer (tamaño del enlace + 1 para '\0')
	bufSize = sb.st_size + 1;
	buffer = malloc(bufSize);
	if (buffer == NULL) {
		perror("Error allocating memory");
		return -1;
	}

	// Lee el contenido del enlace simbólico ORIGEN (no destino)
	n = readlink(orig, buffer, bufSize - 1);
	if (n == -1) {
		perror("Error reading symlink");
		free(buffer);
		return -1;
	}

	// Añade terminador nulo (readlink no lo hace automáticamente)
	buffer[n] = '\0';

	// Crea el nuevo enlace simbólico
	if (symlink(buffer, dest) == -1) {
		perror("Error creating symlink");
		free(buffer);
		return -1;
	}

	free(buffer);
	return 0;
}

int main(int argc, char *argv[])
{
	struct stat sb;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <source file> <destination file>\n", argv[0]);
		return 1;
	}

	// Obtiene información del archivo origen para determinar su tipo
	if (lstat(argv[1], &sb) == -1) {
		perror("Error getting source file information");
		return 1;
	}

	// Determina el tipo de archivo y llama a la función apropiada
	if (S_ISLNK(sb.st_mode)) {
		// Es un enlace simbólico
		printf("Copying symbolic link...\n");
		if (copy_link(argv[1], argv[2]) == -1) {
			fprintf(stderr, "Failed to copy symbolic link\n");
			return 1;
		}
		printf("Symbolic link copied successfully\n");
	} 
	else if (S_ISREG(sb.st_mode)) {
		// Es un archivo regular
		printf("Copying regular file...\n");
		if (copy_regular(argv[1], argv[2]) == -1) {
			fprintf(stderr, "Failed to copy regular file\n");
			return 1;
		}
		printf("Regular file copied successfully\n");
	} 
	else {
		// Otro tipo de archivo (directorio, dispositivo, etc.)
		fprintf(stderr, "Error: Source is not a regular file or symbolic link\n");
		return 1;
	}

	return 0;
}
