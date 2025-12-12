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

	// lstat en lugar de stat para no seguir enlaces simbólicos
	if (lstat(fname, &sb) == -1){
		perror("lstat");
		return -1;
	}
	
	// Inicializa con los bloques del archivo/directorio actual
	*blocks = sb.st_blocks;

	// Si es un directorio, suma recursivamente el contenido
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
	DIR *dir; 					// Puntero al directorio
	struct dirent *entrada;		// Entrada del directorio
	char path[PATH_MAX];		// Ruta completa del archivo
	size_t tam_entrada;  		// Tamaño en bloques de la entrada

	// Abre el directorio
	dir = opendir(dname);
	if (dir == NULL){
		perror("opendir");
		return -1;
	}

	// Lee todas las entradas del directorio
	while ((entrada = readdir(dir)) != NULL){
		// Ignora las entradas . y .. para evitar recursión infinita	
		if (!(strcmp(entrada->d_name, ".")) == 0 && !(strcmp(entrada->d_name, "..") == 0)) {
			// Construye la ruta completa del archivo
			snprintf(path, sizeof(path), "%s/%s", dname, entrada->d_name);
        }

		// Obtiene el tamaño de la entrada (recursivo si es directorio)
		tam_entrada = 0;
		if (get_size(path, &tam_entrada) == 0){
			*blocks += tam_entrada;
		}
	}

	// Cierra el directorio
	closedir(dir);
	return 0;
}

/* Processes all the files in the command line calling get_size on them to
 * obtain the number of 512 B blocks they occupy and prints the total size in
 * kilobytes on the standard output
 */
int main(int argc, char *argv[])
{
	size_t blocks;		// Tamaño en bloques
	size_t kilobytes;	// Tamaño en kilobytes
	int errors = 0;		// Contador de errores

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <file1> [file2] ...\n", argv[0]);
		return 1;
	}

	// Procesa cada archivo/directorio de la línea de comandos
	for (int i = 1; i < argc; i++){
		blocks = 0;

		// Obtiene el tamaño total en bloques
		if (get_size(argv[i], &blocks) == -1){
			fprintf(stderr, "Error getting the file size of %s./n", argv[i]);
			errors++;
		}

		// Convierte a kilobytes (1 KB = 1024 B = 2 bloques de 512 B)
		kilobytes = (blocks + 1) / 2;

		// Muestra el resultado en formato: <tamaño>K <nombre>
		printf("%zuK\t%s\n", kilobytes, argv[i]);
	}

	// Devuelve 0 si no hubo errores, 1 en caso contrario
	return errors > 0 ? 1 : 0;
}


/*Teoria sobre el ejercicio:

- lstat => Se utiliza para obtener información sobre un archivo o enlace simbólico, similar a stat, pero con una diferencia clave: lstat no sigue enlaces simbólicos. Es decir, si el archivo es un enlace simbólico, lstat devuelve información sobre el propio enlace, no sobre el archivo al que apunta.

Se usa para:

Obtener metadatos (permisos, tamaño, tipo, etc.) de archivos y enlaces simbólicos.
Distinguir entre archivos normales y enlaces simbólicos.
Ejemplo de uso típico:

Verificar si un archivo es un enlace simbólico.
Listar información de archivos sin resolver enlaces.
En resumen: lstat sirve para obtener información de archivos, incluyendo enlaces simbólicos, sin seguirlos.

int lstat(const char *pathname, struct stat *statbuf);

Parámetros:

pathname: ruta del archivo o enlace simbólico.
statbuf: puntero a una estructura struct stat donde se almacena la información.
Las propiedades más utilizadas de struct stat tras llamar a lstat son:

st_mode: tipo y permisos del archivo (puedes usar macros como S_ISLNK, S_ISREG, S_ISDIR, etc.).
st_size: tamaño del archivo.
st_nlink: número de enlaces duros.
st_uid y st_gid: propietario y grupo.
st_mtime: fecha de última modificación.
En resumen: lstat no tiene opciones, pero las propiedades más consultadas tras su uso son st_mode (para saber si es enlace, directorio, etc.), st_size, y st_mtime.

lstat se utiliza principalmente en estos escenarios:

Programas que deben distinguir entre archivos normales y enlaces simbólicos, por ejemplo, utilidades tipo ls, find, o scripts de backup.
Cuando necesitas obtener información sobre el propio enlace simbólico (no el archivo al que apunta), como su fecha de creación, permisos, o si existe.
Al recorrer directorios y analizar el tipo de cada entrada, especialmente en herramientas de administración de archivos o sistemas de archivos personalizados.
Para evitar seguir enlaces simbólicos y así prevenir bucles o accesos no deseados (por seguridad).


Struct dirent => Cada vez que usas readdir() para leer el contenido de un directorio
Los campos más importantes de struct dirent son:

d_name: nombre del archivo o subdirectorio (cadena terminada en nulo).
d_ino: número de inodo del archivo.
d_type: tipo de archivo (no siempre disponible, pero útil si lo está).


*/