#include <stdio.h>

int main() {
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
    return 0;
}