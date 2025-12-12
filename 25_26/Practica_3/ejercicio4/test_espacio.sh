#!/bin/bash

# Script de pruebas para espacio.c
# Ejecuta: chmod +x test_espacio.sh && ./test_espacio.sh

echo "========================================="
echo "  Tests para espacio.c"
echo "========================================="
echo ""

# Colores para output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Verificar que el programa existe
if [ ! -f "./espacio" ]; then
    echo -e "${RED}Error: ./espacio no encontrado${NC}"
    echo "Compila primero con: gcc -o espacio espacio.c -Wall"
    exit 1
fi

# Crear directorio de pruebas
TEST_DIR="test_espacio_dir"
rm -rf "$TEST_DIR"
mkdir -p "$TEST_DIR"

echo -e "${YELLOW}Preparando archivos de prueba...${NC}"

# Crear archivos de diferentes tamaños
echo "pequeño" > "$TEST_DIR/small.txt"
dd if=/dev/zero of="$TEST_DIR/medium.txt" bs=1024 count=10 2>/dev/null
dd if=/dev/zero of="$TEST_DIR/large.txt" bs=1024 count=100 2>/dev/null

# Crear subdirectorio con archivos
mkdir -p "$TEST_DIR/subdir"
echo "archivo en subdirectorio" > "$TEST_DIR/subdir/file1.txt"
echo "otro archivo" > "$TEST_DIR/subdir/file2.txt"

# Crear subdirectorio anidado
mkdir -p "$TEST_DIR/subdir/nested"
echo "archivo anidado" > "$TEST_DIR/subdir/nested/deep.txt"

echo ""
echo "========================================="
echo "  Test 1: Archivo simple"
echo "========================================="
echo ""
echo "Tu programa:"
./espacio "$TEST_DIR/small.txt"
echo ""
echo "du -ks:"
du -ks "$TEST_DIR/small.txt"
echo ""

echo "========================================="
echo "  Test 2: Múltiples archivos"
echo "========================================="
echo ""
echo "Tu programa:"
./espacio "$TEST_DIR/small.txt" "$TEST_DIR/medium.txt" "$TEST_DIR/large.txt"
echo ""
echo "du -ks:"
du -ks "$TEST_DIR/small.txt" "$TEST_DIR/medium.txt" "$TEST_DIR/large.txt"
echo ""

echo "========================================="
echo "  Test 3: Directorio simple"
echo "========================================="
echo ""
echo "Tu programa:"
./espacio "$TEST_DIR"
echo ""
echo "du -ks:"
du -ks "$TEST_DIR"
echo ""

echo "========================================="
echo "  Test 4: Subdirectorio"
echo "========================================="
echo ""
echo "Tu programa:"
./espacio "$TEST_DIR/subdir"
echo ""
echo "du -ks:"
du -ks "$TEST_DIR/subdir"
echo ""

echo "========================================="
echo "  Test 5: Comodines (todos los archivos)"
echo "========================================="
echo ""
cd "$TEST_DIR"
echo "Tu programa:"
../espacio *
echo ""
echo "du -ks:"
du -ks *
cd ..
echo ""

echo "========================================="
echo "  Test 6: Directorio actual (.)"
echo "========================================="
echo ""
cd "$TEST_DIR"
echo "Tu programa:"
../espacio .
echo ""
echo "du -ks:"
du -ks .
cd ..
echo ""

echo "========================================="
echo "  Test 7: Archivo inexistente (debe fallar)"
echo "========================================="
echo ""
echo "Tu programa:"
./espacio archivo_que_no_existe.txt
echo ""

echo "========================================="
echo "  Test 8: Sin argumentos (debe mostrar usage)"
echo "========================================="
echo ""
echo "Tu programa:"
./espacio
echo ""

echo "========================================="
echo "  Comparación Automática"
echo "========================================="
echo ""

# Función para comparar resultados
compare_results() {
    local file="$1"
    local my_result=$(./espacio "$file" 2>/dev/null | awk '{print $1}')
    local du_result=$(du -ks "$file" 2>/dev/null | awk '{print $1}')
    
    if [ "$my_result" = "${du_result}K" ]; then
        echo -e "${GREEN}✓${NC} $file: $my_result (correcto)"
    else
        echo -e "${RED}✗${NC} $file: Tu resultado=$my_result, du=$du_result"
    fi
}

echo "Comparando resultados con du -ks:"
echo ""
compare_results "$TEST_DIR/small.txt"
compare_results "$TEST_DIR/medium.txt"
compare_results "$TEST_DIR/large.txt"
compare_results "$TEST_DIR/subdir"
compare_results "$TEST_DIR"

echo ""
echo "========================================="
echo "  Limpieza"
echo "========================================="
echo ""
read -p "¿Borrar directorio de pruebas? (s/n) " -n 1 -r
echo ""
if [[ $REPLY =~ ^[Ss]$ ]]; then
    rm -rf "$TEST_DIR"
    echo "Directorio de pruebas eliminado"
else
    echo "Directorio de pruebas conservado: $TEST_DIR"
fi

echo ""
echo "========================================="
echo "  Tests completados"
echo "========================================="
