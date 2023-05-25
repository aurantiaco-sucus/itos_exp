#include <stdio.h>

int main() {
    // Copy main.c
    FILE *input_file = fopen("main.c", "r");
    if (input_file == NULL) {
        printf("Error: could not open input file.");
        return 1;
    }

    FILE *output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        printf("Error: could not open output file.");
        fclose(input_file);
        return 1;
    }

    char buffer[256];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, sizeof(char), sizeof(buffer), input_file)) > 0) {
        fwrite(buffer, sizeof(char), bytes_read, output_file);
    }

    fclose(input_file);
    fclose(output_file);

    // compare 2 files (main.c and output.txt)
    char buffer_b[256];
    FILE *input_file_b = fopen("main.c", "r");
    FILE *output_file_b = fopen("output.txt", "r");

    while (fread(buffer, sizeof(char), sizeof(buffer), input_file_b) > 0) {
        fread(buffer_b, sizeof(char), sizeof(buffer_b), output_file_b);
        for (int i = 0; i < 256; i++) {
            if (buffer[i] != buffer_b[i]) {
                printf("Error: files do not match.");
                fclose(input_file_b);
                fclose(output_file_b);
                return 1;
            }
        }
    }

    printf("Files match.");
    fclose(input_file_b);
    fclose(output_file_b);

    return 0;
}