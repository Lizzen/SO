#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define BLOCK_SIZE 512

/* Obtiene en el búfer blocks el tamaño del archivo fname utilizando lstat. Si
 * fname es un directorio, se llama a get_size_dir para agregar el tamaño de su contenido.
 */
int get_size(char *fname, size_t *blocks)
{
    struct stat st;

    if (lstat(fname, &st) == -1) {
        perror("Error al obtener información del archivo");
        return -1;
    }

    if (S_ISDIR(st.st_mode)) {
        return get_size_dir(fname, blocks);
    } else {
        *blocks = st.st_blocks;
        return 0;
    }
}

/* Obtiene el número total de bloques ocupados por todos los archivos en un directorio.
 * Si un archivo contenido es un directorio, se realiza una llamada recursiva a get_size_dir.
 * Las entradas "." y ".." se ignoran convenientemente.
 */
int get_size_dir(char *dname, size_t *blocks)
{
    DIR *dir;
    struct dirent *entry;
    char path[PATH_MAX];

    if ((dir = opendir(dname)) == NULL) {
        perror("Error al abrir el directorio");
        return -1;
    }

    *blocks = 0;

    while ((entry = readdir(dir)) != NULL) {
		//Comparar strings, 0 si son iguales, <0 si es menor, >0 si es mayor
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(path, PATH_MAX, "%s/%s", dname, entry->d_name);

        size_t entry_blocks = 0;
        if (get_size(path, &entry_blocks) == -1) {
            closedir(dir);
            return -1;
        }

        *blocks += entry_blocks;
    }

    closedir(dir);
    return 0;
}

/* Procesa todos los archivos en la línea de comandos llamando a get_size en cada uno
 * para obtener el número de bloques de 512 B que ocupan, y luego imprime el tamaño total
 * en kilobytes en la salida estándar.
 */
int main(int argc, char *argv[])
{
    size_t total_blocks = 0;

    for (int i = 1; i < argc; i++) {
        size_t blocks = 0;

        if (get_size(argv[i], &blocks) == -1) {
            fprintf(stderr, "Error al obtener el tamaño del archivo %s\n", argv[i]);
            continue;
        }

        total_blocks += blocks;
        printf("%luK %s\n", (blocks * BLOCK_SIZE) / 1024, argv[i]);
    }

    printf("Total: %luK\n", (total_blocks * BLOCK_SIZE) / 1024);

    return 0;
}
