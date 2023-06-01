#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    FILE* file = NULL;
    int block_size = 1; // Tamaño de bloque predeterminado
    int ret;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
        exit(1);
    }

    if (argc == 3) {
        block_size = atoi(argv[2]);
        if (block_size <= 0) {
            fprintf(stderr, "Block size must be a positive integer.\n");
            exit(1);
        }
    }

    /* Open file */
    if ((file = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "The input file %s could not be opened\n", argv[1]);
        exit(2);
    }

    /* Read file byte by byte */
    unsigned char buffer[block_size];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, block_size, file)) > 0) {
        /* Print bytes to stdout */
        ret = fwrite(buffer, 1, bytes_read, stdout);

        if (ret != bytes_read) {
            fclose(file);
            fprintf(stderr, "fwrite() failed!!\n");
            exit(3);
        }
    }

    fclose(file);
    return 0;
}

