#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 1024

/**
 * Valida que una cadena sea un número entero positivo.
 * @param str Cadena a validar
 * @return El número convertido si es válido, -1 si es inválido
 */
int validate_positive_number(const char *str)
{
	char *endptr;
	long val;

	// Verifica que no esté vacío
	if (str == NULL || *str == '\0') {
		return -1;
	}

	errno = 0;
	// Convierte la cadena a un número entero (long)
	// str: cadena a convertir, &endptr: apuntará al primer carácter no válido, 10: base decimal
	val = strtol(str, &endptr, 10);

	// Verifica errores de conversión
	if (errno != 0 || *endptr != '\0') {
		return -1;
	}

	// Verifica que sea positivo y que quepa en un int
	if (val <= 0 || val > INT_MAX) {
		return -1;
	}

	return (int)val;
}

int main(int argc, char *argv[])
{
	int opt;
	int nFlag = 0;
	int eFlag = 0;
	int nBytes = 0;
	char buffer[BUFFER_SIZE];
	ssize_t bytes_read;
	int fd = -1;
	struct stat file_stat;
	off_t fileSize;
	off_t offset;

	// Procesa opciones de línea de comandos
	while ((opt = getopt(argc, argv, "n:e")) != -1) {
		switch (opt) {
			case 'n':
				nFlag = 1;
				nBytes = validate_positive_number(optarg);
				if (nBytes == -1) {
					fprintf(stderr, "Error: -n requires a positive integer argument\n");
					exit(EXIT_FAILURE);
				}
				break;
			case 'e':
				eFlag = 1;
				break;
			default:
				fprintf(stderr, "Usage: %s [-n num_bytes] [-e] [file]\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	// Verifica que se haya especificado un archivo
	if (optind >= argc) {
		fprintf(stderr, "Error: Expected file after options\n");
		fprintf(stderr, "Usage: %s [-n num_bytes] [-e] [file]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// Abre el archivo para lectura
	fd = open(argv[optind], O_RDONLY);
	if (fd < 0) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	// Obtiene información del archivo
	if (fstat(fd, &file_stat) == -1) {
		perror("Error getting file information");
		close(fd);
		exit(EXIT_FAILURE);
	}

	// Verifica que sea un archivo regular (lseek no funciona con pipes, etc.)
	if (!S_ISREG(file_stat.st_mode)) {
		fprintf(stderr, "Error: File is not a regular file (cannot seek)\n");
		close(fd);
		exit(EXIT_FAILURE);
	}

	// Obtiene el tamaño del archivo
	fileSize = file_stat.st_size;

	// Valida que nBytes no sea mayor que el tamaño del archivo
	if (nFlag && nBytes > fileSize) {
		fprintf(stderr, "Error: Offset (%d bytes) is larger than file size (%lld bytes)\n",
		        nBytes, (long long)fileSize);
		close(fd);
		exit(EXIT_FAILURE);
	}

	// Calcula y establece la posición inicial de lectura
	if (nFlag) {
		if (eFlag) {
			// Modo "end": mostrar los últimos nBytes
			offset = fileSize - nBytes;
		} else {
			// Modo normal: saltar los primeros nBytes
			offset = nBytes;
		}

		if (lseek(fd, offset, SEEK_SET) == -1) {
			perror("Error seeking in file");
			close(fd);
			exit(EXIT_FAILURE);
		}
	}
	// Si no hay flag -n, empieza desde el principio (posición actual = 0)

	// Lee y muestra el contenido del archivo
	while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
		ssize_t bytes_written = 0;
		ssize_t total_written = 0;

		// Asegura que todos los bytes se escriban (write puede escribir menos)
		while (total_written < bytes_read) {
			bytes_written = write(STDOUT_FILENO, buffer + total_written, 
			                      bytes_read - total_written);
			if (bytes_written == -1) {
				perror("Error writing to stdout");
				close(fd);
				exit(EXIT_FAILURE);
			}
			total_written += bytes_written;
		}
	}

	// Verifica errores en la lectura
	if (bytes_read < 0) {
		perror("Error reading file");
		close(fd);
		exit(EXIT_FAILURE);
	}

	// Cierra el archivo
	if (close(fd) == -1) {
		perror("Error closing file");
		exit(EXIT_FAILURE);
	}

	return 0;
}
