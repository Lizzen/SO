#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>

#define BUFFER_SIZE 1024

void display_file(int fd, off_t offset)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Situar el marcador de posición en el desplazamiento especificado
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("Error al ajustar el marcador de posición");
        exit(1);
    }

    // Leer y mostrar el archivo desde la posición actual hasta el final
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        ssize_t bytes_written = write(STDOUT_FILENO, buffer, bytes_read);
        if (bytes_written == -1) {
            perror("Error al escribir en la salida estándar");
            exit(1);
        }
    }

    if (bytes_read == -1) {
        perror("Error al leer el archivo");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    int option;
    int n_flag = 0;
    int e_flag = 0;
    off_t offset = 0;

    // Parsear los argumentos utilizando getopt
    while ((option = getopt(argc, argv, "n:e")) != -1) {
        switch (option) {
            case 'n':
                n_flag = 1;
                offset = atoi(optarg);
                break;
            case 'e':
                e_flag = 1;
                break;
            default:
                fprintf(stderr, "Uso: %s [-n N] [-e] archivo\n", argv[0]);
                exit(1);
        }
    }

    // Verificar que se proporcionó un archivo como argumento
    if (optind >= argc) {
        fprintf(stderr, "Debe especificar un archivo\n");
        exit(1);
    }

    // Abrir el archivo para lectura
    int fd = open(argv[optind], O_RDONLY);
    if (fd == -1) {
        perror("Error al abrir el archivo");
        exit(1);
    }

    // Calcular el desplazamiento del marcador de posición según los flags
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        perror("Error al obtener el tamaño del archivo");
        exit(1);
    }

    if (n_flag) {
        if (e_flag) {
            offset = file_size - offset;
        }
    } else {
        offset = e_flag ? file_size - offset : offset;
    }

    // Mostrar el archivo desde el desplazamiento especificado
    display_file(fd, offset);

    // Cerrar el archivo
    if (close(fd) == -1) {
        perror("Error al cerrar el archivo");
        exit(1);
    }

    return 0;
}


/*Desplazamiento del marcador de posición en ficheros.
En este ejercicio vamos a crear un programa mostrar.c similar al comando cat, que reciba como parámetro el nombre de un fichero y lo muestre por la salida estándar. En este caso asumiremos que es un fichero estándar. Además, nuestro programa recibirá dos argumentos que parsearemos con getopt (consultar su página de manual):

-n N: indica que queremos saltarnos N bytes desde el comienzo del fichero o mostrar únicamente los N últimos bytes del fichero. Que se haga una cosa o la otra depende de la presencia o no de un segundo flag -e. Si el flag -n no aparece N tomará el valor 0.
-e: si aparece, se leerán los últimos N bytes del fichero. Si no aparece, se saltarán los primeros N bytes del fichero.
El programa debe abrir el fichero indicado en la línea de comandos (consultar optinden la página de manual de getopt) y después situar el marcador de posición en la posición correcta antes de leer. Para ello haremos uso de la llamada al sistema lseek (consultar la página de manual). Si el usuario ha usado el flag -e debemos situar el marcador N bytes antes del final del fichero. Si el usuario ha usado el flag -e debemos avanzar el marcador N bytes desde el comienzo del fichero.

Una vez situado el marcador de posición, debemos leer leer byte a byte hasta el final de fichero, escribiendo cada byte leído por la salida estándar (descriptor 1).

Ejercicio 4: Recorrido de directorios.
En este ejercicio vamos a crear un programa espacio.c que reciba una lista de nombres de fichero como parámtros de la llamada, y calculará para cada uno el número total de kilobytes reservados por el sistema para almacenar dicho fichero. En caso de que alguno de los ficheros procesados sean de tipo directorio, se sumarán también los kilobytes ocupados por los ficheros contenidos el directorio (notar que esto es recursivo, porque un directorio puede contener otros directorios).

Para conocer el número de kilobytes reservados por el sistema para almacenar un fichero podemos hacer uso de la llamada a lstat, que nos permite saber el número de bloques de 512 bytes reservados por el sistema.

Para identificar si un fichero es un directorio deberemos hacer una llamada a lstat y consultar el campo st_mode (consultar la página de manual de lstat).

Para recorrer un directorio, primero deberemos abrirlo usando la función de biblioteca opendir y luego leer sus entradas usando la función de biblioteca readdir. Consultar las páginas de manual de estas dos funciones. Notar que las entradas de un directorio no tienen un orden establecido y que todo directorio tiene dos entradas “.” y “..”, que deberemos ignorar si no queremos tener una recursión infinita.

El programa debe mostrar por la salida estándar una línea por fichero de la línea de comandos, con el tamaño total en kilobytes del fichero y el nombre de dicho fichero. Para comprobar si nuestro programa funciona correctamente podemos comparar su salida con la del comando du -ks, pasando a este comando la misma lista de ficheros que al nuestro. Notar que se pueden usar los comodines del shell.

Ejemplo de uso:

$ ls -l .
total 40
-rwxr-xr-x 1 christian christian 20416 Jul 15 12:41 espacio
-rw-r--r-- 1 christian christian  1639 Jul 15 12:41 espacio.c
-rw-r--r-- 1 christian christian  9056 Jul 15 12:41 espacio.o
-rw-r--r-- 1 christian christian   273 Jul 15 09:54 Makefile
$ ./espacio .
44K .
$ ./espacio *
20K espacio
4K  espacio.c
12K espacio.o
4K  Makefile
3 API de procesos e hilos
En esta parte de la práctica trabajaremos las llamadas al sistema: fork, exec, wait, waitpid, getpid, getppid, sigaction, alarm, kill. Además, usaremos las funciones de la biblioteca de pthreads: pthread_create, pthread_join, pthread_self.

Ejercicio 1: Creación de procesos
Diseña un programa fork1.c que cree dos procesos hijos. El primero de ellos no cambiará de ejecutable, pero el segundo sí lo hará, mediante una llamada a execvp. El programa recibirá como parámetros el nombre del ejecutable que deberá ejecutar y los argumentos que necesite pasarle.

El programa realizará una primera llamada a fork. Después de ella, tanto el programa padre como el hijo imprimirán un mensaje indicando si son padre o hijo, su identificador y el de su padre. A continuación, ambos procesos realizarán una segunda llamada a fork, después de la cual cada proceso imprimirá un mensaje indicando si es padre o hijo, su identificador y el de su padre. Cada hijo generado en la segunda llamada cambiará su ejecutable por el que se haya pasado como argumento a main usando execvp. Cada padre esperará a que sus hijos finalicen.

El alumno debe consultar las páginas de manual de las siguientes llamadas al sistema: fork, getpid, getppid, execvp, waitpid.

Para comprobar el funcionamiento correcto de nuestro programa podemos usar como argumento cualquier ejecutable que imprima algo por pantalla, por ejemplo echo o ls.*/