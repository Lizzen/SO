# Guía Paso a Paso: Ejercicio 3 - mostrar.c

## 📚 Objetivo
Crear un programa que muestre el contenido de un archivo por pantalla, permitiendo:
- Mostrar desde un offset específico (`-n N`)
- Mostrar los últimos N bytes (`-n N -e`)
- Leer únicamente archivos regulares

---

## 🎯 Paso 1: Entender el Problema

### ¿Qué debe hacer el programa?
1. Recibir un archivo como argumento
2. Opcionalmente, recibir `-n N` para saltar N bytes o mostrar los últimos N bytes con `-e`
3. Mostrar el contenido del archivo en pantalla
4. Controlar errores de argumentos, apertura, lectura y escritura

### Ejemplo de salida esperada:
```bash
$ ./mostrar archivo.txt
(contenido completo del archivo)

$ ./mostrar -n 10 archivo.txt
(contenido desde el byte 10)

$ ./mostrar -n 20 -e archivo.txt
(últimos 20 bytes del archivo)
```

---

## 🔧 Paso 2: Includes Necesarios

Antes de empezar, añade estos includes (piensa por qué necesitas cada uno):

```c
#include <unistd.h>    // read, write, lseek, getopt
#include <stdlib.h>    // exit, EXIT_FAILURE
#include <stdio.h>     // printf, fprintf, perror
#include <fcntl.h>     // open
#include <sys/types.h> // tipos de datos
#include <sys/stat.h>  // struct stat, fstat, S_ISREG
#include <errno.h>     // manejo de errores
#include <string.h>    // strcmp
```

**Ejercicio:** Investiga qué hace cada función antes de usarla con `man`.

---

## 📝 Paso 3: Procesar Argumentos

### Objetivo:
Analizar las opciones `-n` y `-e` usando `getopt`.

#### 3.1. Analizar opciones
```c
int opt;
int nFlag = 0, eFlag = 0, nBytes = 0;
while ((opt = getopt(argc, argv, "n:e")) != -1) {
    switch (opt) {
        case 'n':
            nFlag = 1;
            // Validar que sea entero positivo
            break;
        case 'e':
            eFlag = 1;
            break;
        default:
            // Mostrar uso y salir
    }
}
```

#### 3.2. Validar argumentos
- Comprobar que hay archivo tras las opciones
- Validar que `nBytes` sea positivo

---

## 📂 Paso 4: Abrir y Validar el Archivo

### Objetivo:
Abrir el archivo y comprobar que es regular.

#### 4.1. Abrir archivo
```c
fd = open(argv[optind], O_RDONLY);
if (fd < 0) { perror("open"); exit(EXIT_FAILURE); }
```

#### 4.2. Comprobar tipo de archivo
```c
struct stat file_stat;
if (fstat(fd, &file_stat) == -1) { perror("fstat"); close(fd); exit(EXIT_FAILURE); }
if (!S_ISREG(file_stat.st_mode)) {
    fprintf(stderr, "No es un archivo regular\n");
    close(fd); exit(EXIT_FAILURE);
}
```

---

## 🏁 Paso 5: Calcular Offset de Lectura

### Objetivo:
Posicionar el puntero de lectura según las opciones.

#### 5.1. Sin `-n`: leer desde el principio
#### 5.2. Con `-n` y sin `-e`: saltar los primeros N bytes
#### 5.3. Con `-n` y `-e`: mostrar los últimos N bytes

```c
if (nFlag) {
    if (eFlag) {
        offset = fileSize - nBytes;
    } else {
        offset = nBytes;
    }
    if (lseek(fd, offset, SEEK_SET) == -1) { perror("lseek"); close(fd); exit(EXIT_FAILURE); }
}
```

---

## 📖 Paso 6: Leer y Mostrar el Archivo

### Objetivo:
Leer el archivo en bloques y mostrarlo por pantalla.

```c
while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
    ssize_t total_written = 0;
    while (total_written < bytes_read) {
        bytes_written = write(STDOUT_FILENO, buffer + total_written, bytes_read - total_written);
        if (bytes_written == -1) { perror("write"); close(fd); exit(EXIT_FAILURE); }
        total_written += bytes_written;
    }
}
if (bytes_read < 0) { perror("read"); close(fd); exit(EXIT_FAILURE); }
```

---

## 🚦 Paso 7: Cerrar el Archivo

```c
if (close(fd) == -1) { perror("close"); exit(EXIT_FAILURE); }
```

---

## 🧪 Paso 8: Compilar y Probar

### 8.1. Compilar
```bash
gcc -o mostrar mostrar.c -Wall
```

### 8.2. Probar
```bash
# Mostrar todo el archivo
./mostrar archivo.txt

# Mostrar desde el byte 10
./mostrar -n 10 archivo.txt

# Mostrar los últimos 20 bytes
./mostrar -n 20 -e archivo.txt
```

---

## 🎓 Conceptos Clave para el Examen

1. Uso de `getopt` para argumentos
2. Validación de archivos regulares con `fstat` y `S_ISREG`
3. Posicionamiento con `lseek`
4. Lectura y escritura robusta con `read` y `write`
5. Manejo de errores en llamadas al sistema

---

## 📋 Checklist de Implementación

- [ ] Analizar argumentos con `getopt`
- [ ] Validar argumentos y archivo
- [ ] Abrir archivo y comprobar tipo
- [ ] Calcular offset con o sin `-n` y `-e`
- [ ] Leer y mostrar el archivo en bloques
- [ ] Manejar errores y cerrar archivo
- [ ] Probar con diferentes combinaciones de opciones

---

## 🔍 Preguntas de Autoevaluación

1. ¿Qué hace la opción `-e`?
    - Permite mostrar los últimos N bytes del archivo en vez de saltar los primeros N bytes. Se usa junto con `-n`.

2. ¿Por qué se valida que el archivo sea regular?
    - Porque operaciones como `lseek` solo funcionan correctamente en archivos regulares. No se puede garantizar el comportamiento en pipes, sockets, etc.

3. ¿Qué ocurre si `nBytes` es mayor que el tamaño del archivo?
    - El programa muestra un error y termina, ya que no tiene sentido saltar o mostrar más bytes de los que existen en el archivo.

4. ¿Por qué se usa un bucle para `write`?
    - Porque la llamada a `write` puede escribir menos bytes de los solicitados. El bucle asegura que se escriban todos los bytes leídos hasta completar el bloque.

5. ¿Qué pasa si no se valida el resultado de `lseek`?
    - Si `lseek` falla y no se comprueba, el programa podría leer desde una posición incorrecta o fallar silenciosamente, mostrando resultados erróneos o provocando errores posteriores.

---

## 💡 Tips para el Examen

1. Siempre verifica errores en llamadas al sistema
2. Usa mensajes claros en `perror` y `fprintf`
3. Recuerda validar argumentos y tipos de archivo
4. Usa `lseek` para posicionar el puntero de lectura
5. Lee y escribe en bloques para eficiencia

---

## 📚 Páginas de Manual a Consultar

```bash
man 2 open
man 2 read
man 2 write
man 2 lseek
man 2 fstat
man 3 getopt
```

---

¡Buena suerte con tu práctica! 🚀
