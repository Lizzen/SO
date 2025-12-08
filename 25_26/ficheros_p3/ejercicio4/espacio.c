#include <stdio.h>      // printf, fprintf, perror
#include <stdlib.h>     // exit, EXIT_FAILURE
#include <sys/types.h>  // Tipos de datos del sistema
#include <sys/stat.h>   // struct stat, lstat(), S_ISDIR()
#include <dirent.h>     // DIR, opendir(), readdir(), closedir()
#include <string.h>     // strcmp(), snprintf()
#include <unistd.h>     // Constantes del sistema

/* Forward declaration */
int get_size_dir(char *fname, size_t *blocks);

/* Gets in the blocks buffer the size of file fname using lstat. If fname is a
 * directory get_size_dir is called to add the size of its contents.
 */
int get_size(char *fname, size_t *blocks)
{
	struct stat sb;

	if (lstat(fname, &sb) == -1){
		perror("lstat");
		return -1;
	}

	*blocks = sb.st_blocks;

	if (S_ISDIR(sb.st_mode)){
		if (get_size_dir(fname, blocks) == -1){
			return -1;
		}
	}

	return 0;
}


/* Gets the total number of blocks occupied by all the files in a directory. If
 * a contained file is a directory a recursive call to get_size_dir is
 * performed. Entries . and .. are conveniently ignored.
 */
int get_size_dir(char *dname, size_t *blocks)
{

}

/* Processes all the files in the command line calling get_size on them to
 * obtain the number of 512 B blocks they occupy and prints the total size in
 * kilobytes on the standard output
 */
int main(int argc, char *argv[])
{
	size_t blocks;
	size_t kilobytes;
	int errors = 0;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <file1> [file2] ...\n", argv[0]);
		return 1;
	}

	for (int i = 0; i < argc; i++){
		blocks = 0;

		if (get_size(argv[i], blocks) == -1){
			fprintf(stderr, "Error getting the file size of %s./n", argv[i]);
			errors++;
		}

		kilobytes = (blocks + 1) / 2;

		// Muestra el resultado en formato: <tamaño>K <nombre>
		printf("%zuK\t%s\n", kilobytes, argv[i]);
	}

	return 0;
}
