# Guía Paso a Paso: Ejercicio 4 - espacio.c

## 📚 Objetivo
Crear un programa que calcule el espacio en disco ocupado por archivos y directorios de forma recursiva, similar a `du -ks`.

---

## 🎯 Paso 1: Entender el Problema

### ¿Qué debe hacer el programa?
1. Recibir archivos/directorios como argumentos
2. Calcular cuántos **bloques de 512 bytes** ocupa cada uno
3. Si es un directorio, sumar recursivamente todo su contenido
4. Mostrar el resultado en **kilobytes**

### Ejemplo de salida esperada:
```bash
$ ./espacio archivo.txt directorio/
4K    archivo.txt
48K   directorio/
```

---

## 🔧 Paso 2: Includes Necesarios

Antes de empezar, añade estos includes (piensa por qué necesitas cada uno):

```c
#include <stdio.h>      // printf, fprintf, perror
#include <stdlib.h>     // exit, EXIT_FAILURE
#include <sys/types.h>  // Tipos de datos del sistema
#include <sys/stat.h>   // struct stat, lstat(), S_ISDIR()
#include <dirent.h>     // DIR, opendir(), readdir(), closedir()
#include <string.h>     // strcmp(), snprintf()
#include <unistd.h>     // Constantes del sistema
```

**Ejercicio:** Investiga qué hace cada función antes de usarla con `man`.

---

## 📝 Paso 3: Implementar `get_size()`

### Objetivo de la función:
Obtener el tamaño de UN archivo o directorio (si es directorio, llamar a `get_size_dir`).

### Pasos a seguir:

#### 3.1. Declarar variables necesarias
```c
int get_size(char *fname, size_t *blocks)
{
    struct stat sb;  // ¿Qué información contiene struct stat?
    
    // Tu código aquí...
}
```

**Pregunta:** ¿Por qué usamos `struct stat`? ¿Qué campos tiene?
**Respuesta:** Consulta `man 2 lstat` y busca los campos importantes.

#### 3.2. Obtener información del archivo con `lstat()`
```c
// Usa lstat() para obtener información del archivo
// ¿Por qué lstat() y no stat()? Pista: enlaces simbólicos
if (lstat(fname, &sb) == -1) {
    // ¿Qué hacer si hay error?
    // Pista: usa perror() para mostrar el error
    return -1;
}
```

**Ejercicio:** Investiga la diferencia entre `stat()` y `lstat()`.

#### 3.3. Obtener los bloques del archivo
```c
// El campo st_blocks contiene el número de bloques de 512B
*blocks = sb.st_blocks;  // ¿Por qué usamos puntero?
```

**Pregunta:** ¿Por qué pasamos `blocks` como puntero y no por valor?

#### 3.4. Verificar si es un directorio
```c
// Si es un directorio, necesitamos sumar su contenido
if (S_ISDIR(sb.st_mode)) {
    // Llamar a get_size_dir() para procesar el directorio
    // ¿Qué parámetros necesita?
}
```

**Ejercicio:** Investiga qué es `st_mode` y qué macros existen (`S_ISREG`, `S_ISLNK`, etc.).

#### 3.5. Retornar éxito
```c
return 0;  // 0 = éxito, -1 = error
```

---

## 📂 Paso 4: Implementar `get_size_dir()`

### Objetivo de la función:
Recorrer TODAS las entradas de un directorio y sumar sus tamaños.

### Pasos a seguir:

#### 4.1. Declarar variables necesarias
```c
int get_size_dir(char *dname, size_t *blocks)
{
    DIR *dir;                    // Puntero al directorio
    struct dirent *entry;        // Entrada del directorio
    char path[PATH_MAX];         // Ruta completa del archivo
    size_t entry_blocks;         // Bloques de cada entrada
    
    // Tu código aquí...
}
```

**Pregunta:** ¿Qué es `struct dirent`? ¿Qué campos tiene?
**Respuesta:** Consulta `man 3 readdir`.

#### 4.2. Abrir el directorio
```c
dir = opendir(dname);
if (dir == NULL) {
    // ¿Qué hacer si no se puede abrir?
    perror("opendir");
    return -1;
}
```

**Ejercicio:** Prueba qué pasa si intentas abrir un archivo que no es directorio.

#### 4.3. Leer todas las entradas del directorio
```c
while ((entry = readdir(dir)) != NULL) {
    // readdir() retorna NULL cuando no hay más entradas
    
    // IMPORTANTE: Ignorar "." y ".."
    // ¿Por qué? ¿Qué pasaría si no los ignoramos?
    if (strcmp(entry->d_name, ".") == 0 || 
        strcmp(entry->d_name, "..") == 0) {
        continue;  // Saltar esta entrada
    }
    
    // Tu código aquí...
}
```

**Pregunta CRÍTICA:** ¿Qué pasaría si NO ignoramos "." y ".."?
**Respuesta:** Recursión infinita. "." apunta al directorio actual.

#### 4.4. Construir la ruta completa
```c
// Si dname = "dir" y entry->d_name = "file.txt"
// Necesitamos construir "dir/file.txt"
snprintf(path, sizeof(path), "%s/%s", dname, entry->d_name);
```

**Ejercicio:** ¿Por qué usamos `snprintf()` y no `sprintf()`?

#### 4.5. Obtener el tamaño de cada entrada
```c
entry_blocks = 0;
if (get_size(path, &entry_blocks) == 0) {
    *blocks += entry_blocks;  // Acumular bloques
}
// Si hay error, continuar con el siguiente archivo
```

**Pregunta:** ¿Por qué esta función es recursiva?
**Respuesta:** `get_size()` puede llamar a `get_size_dir()`, que llama a `get_size()` de nuevo.

#### 4.6. Cerrar el directorio
```c
if (closedir(dir) == -1) {
    perror("closedir");
    return -1;
}

return 0;
```

---

## 🚀 Paso 5: Implementar `main()`

### Objetivo:
Procesar los argumentos de la línea de comandos y mostrar resultados.

### Pasos a seguir:

#### 5.1. Declarar variables
```c
int main(int argc, char *argv[])
{
    size_t blocks;      // Bloques totales
    size_t kilobytes;   // KB totales
    int i;              // Contador
    int errors = 0;     // Contador de errores
    
    // Tu código aquí...
}
```

#### 5.2. Validar argumentos
```c
if (argc < 2) {
    fprintf(stderr, "Usage: %s <file1> [file2] ...\n", argv[0]);
    return 1;
}
```

#### 5.3. Procesar cada archivo
```c
for (i = 1; i < argc; i++) {
    blocks = 0;  // Reiniciar para cada archivo
    
    // Obtener tamaño total
    if (get_size(argv[i], &blocks) == -1) {
        fprintf(stderr, "Error processing '%s'\n", argv[i]);
        errors++;
        continue;  // Continuar con el siguiente
    }
    
    // Convertir bloques a KB
    // 1 bloque = 512 bytes
    // 1 KB = 1024 bytes
    // KB = (bloques * 512) / 1024 = bloques / 2
    kilobytes = (blocks + 1) / 2;  // +1 para redondear hacia arriba
    
    // Mostrar resultado
    printf("%zuK\t%s\n", kilobytes, argv[i]);
}

return errors > 0 ? 1 : 0;
```

**Pregunta:** ¿Por qué `(blocks + 1) / 2` y no `blocks / 2`?
**Respuesta:** Para redondear hacia arriba (como hace `du`).

---

## 🧪 Paso 6: Compilar y Probar

### 6.1. Compilar
```bash
gcc -o espacio espacio.c -Wall
```

**Ejercicio:** ¿Qué hace la opción `-Wall`?

### 6.2. Probar con archivos simples
```bash
# Crear archivo de prueba
echo "Hola mundo" > test.txt

# Probar
./espacio test.txt

# Comparar con du
du -ks test.txt
```

### 6.3. Probar con directorios
```bash
./espacio .
du -ks .

# ¿Coinciden los resultados?
```

### 6.4. Probar con múltiples argumentos
```bash
./espacio * 
du -ks *
```

---

## 🎓 Conceptos Clave para el Examen

### 1. `lstat()` vs `stat()`
- **`stat()`**: Sigue enlaces simbólicos
- **`lstat()`**: NO sigue enlaces simbólicos (obtiene info del enlace mismo)

### 2. Recorrido de directorios
```c
DIR *dir = opendir(path);
struct dirent *entry;
while ((entry = readdir(dir)) != NULL) {
    // Procesar entry->d_name
}
closedir(dir);
```

### 3. Ignorar "." y ".."
```c
if (strcmp(entry->d_name, ".") == 0 || 
    strcmp(entry->d_name, "..") == 0) {
    continue;
}
```

### 4. Recursión en directorios
```
get_size("dir")
  └─ get_size_dir("dir")
      ├─ get_size("dir/file1")
      └─ get_size("dir/subdir")
          └─ get_size_dir("dir/subdir")  ← Recursión
```

### 5. Conversión de bloques a KB
```c
// 1 bloque = 512 bytes
// 1 KB = 1024 bytes
kilobytes = (blocks + 1) / 2;  // Redondeo hacia arriba
```

---

## 📋 Checklist de Implementación

Marca cada paso cuando lo completes:

- [ ] Añadir todos los includes necesarios
- [ ] Implementar `get_size()`:
  - [ ] Llamar a `lstat()`
  - [ ] Verificar errores
  - [ ] Obtener `st_blocks`
  - [ ] Verificar si es directorio con `S_ISDIR()`
  - [ ] Llamar a `get_size_dir()` si es necesario
- [ ] Implementar `get_size_dir()`:
  - [ ] Abrir directorio con `opendir()`
  - [ ] Loop con `readdir()`
  - [ ] Ignorar "." y ".."
  - [ ] Construir ruta completa con `snprintf()`
  - [ ] Llamar recursivamente a `get_size()`
  - [ ] Cerrar directorio con `closedir()`
- [ ] Implementar `main()`:
  - [ ] Validar argumentos
  - [ ] Loop sobre `argv[]`
  - [ ] Llamar a `get_size()`
  - [ ] Convertir bloques a KB
  - [ ] Mostrar resultado con `printf()`
- [ ] Compilar sin warnings
- [ ] Probar con archivos simples
- [ ] Probar con directorios
- [ ] Comparar con `du -ks`

---

## 🔍 Preguntas de Autoevaluación

1. ¿Qué diferencia hay entre `stat()` y `lstat()`?
    - `stat()` sigue enlaces simbólicos y da información del archivo al que apuntan, mientras que `lstat()` da información del propio enlace simbólico.

2. ¿Qué información contiene `struct stat`?
    - Contiene metadatos del archivo: tipo, permisos, tamaño, número de bloques, propietario, fechas de acceso/modificación, etc.

3. ¿Qué es `st_blocks` y en qué unidades está?
    - Es el número de bloques de 512 bytes que ocupa el archivo en disco.

4. ¿Por qué debemos ignorar "." y ".."?
    - Porque representan el directorio actual y el padre. No ignorarlos causaría recursión infinita al procesar directorios.

5. ¿Cómo funciona la recursión en este programa?
    - Cuando se encuentra un directorio, se llama recursivamente a la función para procesar su contenido, sumando los bloques de todos los archivos y subdirectorios.

6. ¿Cómo se convierte bloques de 512B a KB?
    - Dividiendo el número de bloques entre 2 (o usando `(blocks + 1) / 2` para redondear hacia arriba).

7. ¿Qué hace `readdir()` cuando no hay más entradas?
    - Devuelve `NULL`, indicando que se han leído todas las entradas del directorio.

8. ¿Por qué usamos `snprintf()` para construir rutas?
    - Porque es más seguro que `sprintf()`, ya que evita desbordamientos de buffer al limitar el tamaño de la cadena resultante.

---

## 💡 Tips para el Examen

1. **Siempre verifica errores** en llamadas al sistema
2. **Usa `perror()`** para mensajes de error descriptivos
3. **Recuerda ignorar "." y ".."** en directorios
4. **Construye rutas completas** con `snprintf()`
5. **Usa `lstat()` no `stat()`** para no seguir symlinks
6. **La recursión es clave**: `get_size()` ↔ `get_size_dir()`

---

## 📚 Páginas de Manual a Consultar

```bash
man 2 lstat      # Información de archivos
man 2 stat       # Diferencia con lstat
man 3 opendir    # Abrir directorios
man 3 readdir    # Leer entradas de directorios
man 3 closedir   # Cerrar directorios
man 3 snprintf   # Construcción segura de strings
```

---

## 🎯 Ejercicios Adicionales

1. **Modificar** el programa para mostrar también el número de archivos
2. **Añadir** una opción `-h` para mostrar tamaños en formato humano (KB, MB, GB)
3. **Implementar** una opción para seguir enlaces simbólicos
4. **Añadir** contador de directorios procesados

---

¡Buena suerte con tu práctica! 🚀
