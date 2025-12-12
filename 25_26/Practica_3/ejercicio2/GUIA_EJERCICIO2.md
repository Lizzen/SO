# Guía Paso a Paso: Ejercicio 2 - copy2.c

## 📚 Objetivo
Crear un programa en C que copie archivos regulares y enlaces simbólicos, diferenciando el tratamiento de cada tipo.

---

## 🎯 Paso 1: Entender el Problema

### ¿Qué debe hacer el programa?
1. Recibir dos argumentos: archivo origen y archivo destino
2. Detectar si el archivo origen es regular o un enlace simbólico
3. Copiar el contenido si es regular, o duplicar el enlace si es simbólico
4. Mostrar mensajes claros de éxito o error

### Ejemplo de salida esperada:
```bash
$ ./copy2 archivo.txt copia.txt
Copying regular file...
Regular file copied successfully

$ ./copy2 enlace.lnk copia.lnk
Copying symbolic link...
Symbolic link copied successfully
```

---

## 🔧 Paso 2: Includes Necesarios

```c
#include <stdio.h>      // printf, fprintf, perror
#include <stdlib.h>     // exit, malloc, free
#include <fcntl.h>      // open, O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC
#include <unistd.h>     // read, write, close, lseek, symlink, readlink
#include <sys/types.h>  // tipos de datos
#include <sys/stat.h>   // struct stat, lstat, S_ISREG, S_ISLNK
#include <string.h>     // strcpy, strlen
```

---

## 📝 Paso 3: Analizar Argumentos y Tipo de Archivo

### 3.1. Validar argumentos
- El programa debe recibir exactamente dos argumentos (origen y destino)

### 3.2. Obtener información del archivo origen
- Usar `lstat()` para saber si es regular o enlace simbólico

### 3.3. Seleccionar función de copia
- Si es regular: usar `copy_regular`
- Si es enlace simbólico: usar `copy_link`
- Si no es ninguno: mostrar error

---

## 📂 Paso 4: Copiar un Archivo Regular

### 4.1. Abrir archivos
- Abrir el archivo origen en modo solo lectura
- Abrir/crear el archivo destino en modo escritura, truncando si existe

### 4.2. Copiar contenido
- Leer en bloques (buffer de 1024 bytes)
- Escribir cada bloque en el destino
- Comprobar errores de lectura y escritura

### 4.3. Cerrar archivos
- Cerrar ambos descriptores y comprobar errores

---

## 🔗 Paso 5: Copiar un Enlace Simbólico

### 5.1. Obtener información del enlace
- Usar `lstat()` para conocer el tamaño del enlace

### 5.2. Leer el destino del enlace
- Reservar memoria suficiente
- Usar `readlink()` para leer el contenido del enlace simbólico

### 5.3. Crear el nuevo enlace
- Usar `symlink()` con el contenido leído y la ruta destino

### 5.4. Liberar memoria
- Liberar el buffer reservado

---

## 🚦 Paso 6: Manejo de Errores
- Comprobar el valor de retorno de todas las llamadas al sistema
- Mostrar mensajes claros con `perror` o `fprintf(stderr, ...)`
- Cerrar archivos abiertos aunque haya error

---

## 🧪 Paso 7: Compilar y Probar

### 7.1. Compilar
```bash
gcc -o copy2 copy2.c -Wall
```

### 7.2. Probar
```bash
# Copiar archivo regular
./copy2 archivo.txt copia.txt

# Copiar enlace simbólico
ln -s archivo.txt enlace.lnk
./copy2 enlace.lnk copia.lnk

# Probar con archivo no regular
mkdir dir
./copy2 dir copia_dir
```

---

## 🎓 Conceptos Clave para el Examen

1. Diferencia entre `stat()` y `lstat()`
2. Uso de `read`, `write`, `open`, `close` para copiar archivos
3. Uso de `readlink` y `symlink` para enlaces simbólicos
4. Manejo de errores en llamadas al sistema
5. Liberación de recursos (memoria y descriptores)

---

## 📋 Checklist de Implementación

- [ ] Validar argumentos
- [ ] Usar `lstat()` para detectar tipo de archivo
- [ ] Copiar archivo regular con lectura/escritura en bloques
- [ ] Copiar enlace simbólico con `readlink` y `symlink`
- [ ] Manejar errores y liberar recursos
- [ ] Probar con archivos y enlaces

---

## 🔍 Preguntas de Autoevaluación

1. ¿Por qué se usa `lstat()` y no `stat()` para detectar enlaces simbólicos?
	- Porque `lstat()` permite obtener información del propio enlace simbólico, mientras que `stat()` sigue el enlace y da información del archivo al que apunta.

2. ¿Qué ocurre si el archivo destino ya existe?
	- El archivo destino se trunca (se borra su contenido) y se sobrescribe con el nuevo contenido, debido al uso de la bandera `O_TRUNC` en `open()`.

3. ¿Por qué se reserva memoria dinámica en `copy_link`?
	- Para almacenar temporalmente el contenido (ruta de destino) del enlace simbólico, cuyo tamaño se obtiene con `lstat()`.

4. ¿Qué pasa si no se cierran los descriptores de archivo?
	- Se pueden producir fugas de recursos (file descriptor leaks), lo que puede agotar el número de archivos abiertos permitidos por el sistema y provocar errores en el programa o en otros procesos.

5. ¿Por qué se comprueba el valor de retorno de `read` y `write`?
	- Para detectar y manejar errores de lectura o escritura, asegurando que los datos se copian correctamente y que el programa puede informar o reaccionar ante fallos del sistema.

---

## 💡 Tips para el Examen

1. Siempre comprueba errores en llamadas al sistema
2. Usa mensajes claros en los errores
3. Libera memoria y cierra archivos aunque haya error
4. Recuerda que `readlink` no añade terminador nulo
5. Usa permisos adecuados al crear archivos

---

## 📚 Páginas de Manual a Consultar

```bash
man 2 open
man 2 read
man 2 write
man 2 close
man 2 lstat
man 2 readlink
man 2 symlink
```

---

¡Buena suerte con tu práctica! 🚀
