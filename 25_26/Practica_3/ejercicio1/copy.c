#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

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

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <source file> <destination file>\n", argv[0]);
		return 1;
	}

	// Abre el archivo origen
	int fdin = open(argv[1], O_RDONLY);
	if (fdin == -1) {
		perror("Error opening source file");
		return 1;
	}

	// Abre el archivo destino
	int fdout = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fdout == -1) {
		perror("Error opening destination file");
		close(fdin);  // Cierra el archivo origen antes de salir
		return 1;
	}

	// Realiza la copia
	if (copy(fdin, fdout) == -1) {
		fprintf(stderr, "Copy operation failed\n");
		return 1;
	}

	return 0;
}
