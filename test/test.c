// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../includes/main.h"

int main() {
    char letter;
    FILE *fp;

    // Crear un archivo por cada letra del alfabeto
    for (letter = 'a'; letter <= 'z'; letter++) {
        char filename[20];
        sprintf(filename, "%c.txt", letter);

        // Abrir el archivo
        fp = fopen(filename, "w");

        // Escribir la letra en el archivo
        fprintf(fp, "%c", letter);

        // Cerrar el archivo
        fclose(fp);
    }

    return 0;
};
